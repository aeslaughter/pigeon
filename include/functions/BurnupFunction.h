#ifndef BURNUPFUNCTION_H
#define BURNUPFUNCTION_H

#include "Function.h"
#include "FunctionInterface.h"

#include <vector>
#include <algorithm>

class BurnupFunction : public Function, FunctionInterface
{
private:

//
// Bessel functions from:
// Handbook of Mathematical Functions, M. Abramowitz and I. Stegun
//

  Real
  I0( Real x )
  {
    Real val(0);
    if (x < 3.75)
    {
      const Real X(x*x/3.75/3.75);
      val = 1.+
        X*(3.5156229+X
           *(3.0899424+X
             *(1.2067492+X
               *(0.2659732+X
                 *(0.0360768+X
                   * 0.0045813)))));
    }
    else
    {
      const Real X(3.75/x);
      val =   ( 0.39894228+X
                *( 0.01328592+X
                   *( 0.00225319+X
                      *(-0.00157565+X
                        *( 0.00916281+X
                           *(-0.02057706+X
                             *( 0.02635537+X
                                *(-0.01647633+X
                                  *  0.00392377))))))))*std::exp(x)/std::sqrt(x);
    }
    return val;
  }

  Real
  I1( Real x )
  {
    Real val(0);
    if (x < 3.75)
    {
      const Real X(x*x/3.75/3.75);
      val =  (0.50000000+X
              *(0.87890594+X
                *(0.51498869+X
                  *(0.15084934+X
                    *(0.02658733+X
                      *(0.00301532+X
                        * 0.00032411))))))*x;
    }
    else
    {
      const Real X(3.75/x);
      val =  ( 0.39894228+X
               *(-0.03988024+X
                 *(-0.00362018+X
                   *( 0.00163801+X
                      *(-0.01031555+X
                        *( 0.02282967+X
                           *(-0.02895312+X
                             *( 0.01787654+X
                                * -0.00420059))))))))*std::exp(x)/std::sqrt(x);
    }
    return val;
  }

  Real
  K0( Real x )
  {
    Real val(0);
    if (x < 2)
    {
      const Real X(x*x/4);
      val =  (-0.57721566+X
              *( 0.42278420+X
                 *( 0.23069756+X
                    *( 0.03488590+X
                       *( 0.00262698+X
                          *( 0.00010750+X
                             *  0.00000740))))))-std::log(x/2)*I0(x);
    }
    else
    {
      const Real X(2/x);
      val =  ( 1.25331414+X
               *(-0.07832358+X
                 *( 0.02189568+X
                    *(-0.01062446+X
                      *( 0.00587872+X
                         *(-0.00251540+X
                           *  0.00053208))))))/std::exp(x)/std::sqrt(x);
    }
    return val;
  }

  Real
  K1( Real x )
  {
    Real val(0);
    if (x < 2)
    {
      const Real X(x*x/4);
      val =  ( 1.00000000+X
               *( 0.15443144+X
                  *(-0.67278579+X
                    *(-0.18156897+X
                      *(-0.01919402+X
                        *(-0.00110404+X
                          * -0.00004686))))))/x+std::log(x/2)*I1(x);
    }
    else
    {
      const Real X(2/x);
      val =  ( 1.25331414+X
               *( 0.23498619+X
                  *(-0.03655620+X
                    *( 0.01504268+X
                       *(-0.00780353+X
                         *( 0.00325614+X
                            * -0.00068245))))))/std::exp(x)/std::sqrt(x);
    }
    return val;
  }

  Real p1() const;
  std::vector<Real> sigma_f() const;
  std::vector<Real> sigma_c() const;
  std::vector<Real> sigma_a(const std::string & name) const;

  void
  computeRadialLocations( const Real r_inner, const Real r_outer,
                          const Real bias,
                          std::vector<Real> & r )
  {
    const unsigned num_radial( r.size() );
    // Total length (r_outer-r_inner) = Length of first segment * Sum(i=1,num segments) (bias^(i-1))
    // Total length (r_outer-r_inner) = Length of first segment * (bias^0 + bias^1 + bias^2 + ... bias^(num segments-1))
    // Total length (r_outer-r_inner) = Length of first segment * (1 + bias * (1 + bias * (1 + bias * ( ... ))))
    // Number of segments = num_radial-1
    // Summation below requires segments-2 passes
    Real sum(1+bias);
    for (unsigned int i(0); i < num_radial-3; ++i)
    {
      sum = 1 + bias*sum;
    }
    const Real initial_length( (r_outer-r_inner) / sum );
    r[0] = r_inner;
    r[1] = r[0] + initial_length;
    for (unsigned int i(2); i < num_radial-1; ++i)
    {
      r[i] = r[i-1] + bias*(r[i-1]-r[i-2]);
    }
    r[num_radial-1] = r_outer;
  }

  void
  computeVolumeFractions( const Real height,
                          const unsigned int /*num_axial*/,
                          const std::vector<Real> & r,
                          std::vector<Real> & volume_frac )
  {

    const unsigned num_radial( r.size() );
    {
      std::vector<Real> tmp( num_radial, 0.0 );
      tmp.swap( volume_frac );
    }

    Real volume(0);

    for (unsigned int i(0); i < num_radial-1; ++i)
    {
      const Real wi((2*r[i]+r[i+1])/(3*(r[i]+r[i+1])));
      const Real wo(1-wi);
      const Real v(height * M_PI * (r[i+1]*r[i+1] - r[i]*r[i]));
      volume += v;
      volume_frac[i] += wi * v;
      volume_frac[i+1] += wo * v;
    }
    for (unsigned int i(0); i < num_radial; ++i)
    {
      volume_frac[i] /= volume;
    }

//     Moose::out << "Data:\n"
//                << "\tRing vol:  " << volume << "\n"
//                << "\tVol_frac at r:\n";
//     for (unsigned int i(0); i < num_radial; ++i)
//     {
//       Moose::out << "\t\t" << i << "\t" << r[i] << "\t" << volume_frac[i]*volume << "\t" << volume_frac[i] << "\n";
//     }
//     const Real true_vol(M_PI*(r[num_radial-1]*r[num_radial-1]-r[0]*r[0])*height);
//     Moose::out << "\tVol error: " << (true_vol-volume)/true_vol*100 << "%"
//                << std::endl;
#if 0

    const unsigned num_radial( r.size() );
    std::vector<Real> volume(num_radial);
    {
      const Real rO( r[1] + r[0] );
      const Real rI( r[0] );
      volume[0] = M_PI * ( 0.25 * rO*rO - rI*rI ) * height;
    }
    for (unsigned int i(1); i < num_radial-1; ++i)
    {
      const Real rO( r[i+1] + r[i] );
      const Real rI( r[i] + r[i-1] );
      volume[i] = 0.25 * M_PI * (rO*rO - rI*rI) * height;
    }
    {
      const Real rI( r[num_radial-1] + r[num_radial-2] );
      volume[num_radial-1] = M_PI * ( r[num_radial-1]*r[num_radial-1] - 0.25 * rI*rI ) * height;
    }

    Real ring_volume(0);
    for ( unsigned int i(0); i < num_radial; ++i )
    {
      ring_volume += volume[i];
    }
    for ( unsigned int i(0); i < num_radial; ++i )
    {
      volume_frac[i] = volume[i] / ring_volume;
    }

//     Moose::out << "Data:\n"
//                << "\tRing vol:  " << ring_volume << "\n"
//                << "\tVol at r:\n";
//     for (unsigned int i(0); i < num_radial; ++i)
//     {
//       Moose::out << "\t\t" << i << "\t" << r[i] << "\t" << volume[i] << "\t" << volume_frac[i] << "\n";
//     }
//     const Real total_vol(ring_volume * (num_axial-1));
//     Moose::out << "\tTotal vol: " << total_vol << "\n";
//     const Real true_vol(M_PI*(r[num_radial-1]*r[num_radial-1]-r[0]*r[0])*height*(num_axial-1));
//     Moose::out << "\tVol error: " << (true_vol-total_vol)/true_vol*100 << "%"
//                << std::endl;
#endif
  }

  void
  bounds(Real target,
         const std::vector<Real> & input,
         unsigned int & lower,
         unsigned int & upper)
  {
    if ( target <= input[0] )
    {
      lower = upper = 0;
      return;
    }
    const unsigned len(input.size());
    if ( target >= input[len-1] )
    {
      lower = upper = len-1;
      return;
    }
    const Real * start = &input[0];
    const Real * end = start + len;
    const Real * p = std::lower_bound( start, end, target );
    lower = p - start - 1;
    upper = lower + 1;
  }

  Real
  sample( const Point & pt,
          const std::vector<Real> & radius,
          const std::vector<Real> & height,
          const std::vector<std::vector<Real> > & data ); // [height][radius]

  void
  computeFissionRate( const Real time );

  void
  computeBurnup( const Real delta_t );

  void
  computeRadialPowerFactor();

  enum IsoIndex
  {
    N235, N238, N239, N240, N241, N242, NUM_ISOTOPES
  };

  enum OutputIndex
  {
    BURNUP=NUM_ISOTOPES, FISSION_RATE, RPF, NUM_OUTPUTS
  };


  Function * const _ralp;
  Function * const _axial_profile;
  const Real _value;
  const bool _rpf_active;
  const unsigned _num_radial;
  const unsigned _num_axial;
  const unsigned _axial_axis;
  const Real _a_lower;
  const Real _a_upper;
  const Real _height;
  const Real _r_inner;
  const Real _r_outer;
  const Real _bias;
  const Real _fuel_volume_ratio;
  Real & _time_last_requested;
  const Real _density;
  const Real _porosity_fraction;
  const Real _energy_per_fission;
  const Real _ntot;
  Real _p_to_fr;
  std::vector<Real> _r;
  std::vector<Real> _h;
  std::vector<Real> _volume_frac;
  const Real _p1;
  const std::vector<Real> _i_enrich;
  const std::vector<Real> _sigma_f;
  const std::vector<Real> _sigma_c;
  const std::vector<Real> _sigma_a;
  const std::vector<Real> _sigma_a_thermal;

  // Index [i][j][k] -> [isotope][height][radius]
  std::vector<std::vector<std::vector<Real> > > & _concentration; // Reference for restart
  std::vector<std::vector<std::vector<Real> > > _concentration_old;

  // Index [i][j] -> [isotope][height]
  std::vector<std::vector<Real> > & _average_concentration;  // Reference for restart
  std::vector<std::vector<Real> > & _average_concentration_old;  // Reference for restart; yes, need old

  // burnup is MW*d/kgU    [height][radius]
  std::vector<std::vector<Real> > & _burnup; // Reference for restart
  std::vector<std::vector<Real> > _burnup_old;

  std::vector<std::vector<Real> > _fission_rate;
  std::vector<std::vector<Real> > _fission_rate_old;

  // Index [i][j] -> [height][radius]
  std::vector<std::vector<Real> > & _rpf; // Reference for restart

  std::vector<Real> _f_normalized;

  const Real _MWdperFission;
  const Real _fisskgU_per_MWdaU;

public:

  BurnupFunction(const InputParameters & parameters);

  void setup(const Real delta_t, const Real time);

  virtual Real value(Real t, const Point & pt);

  Real n235( const Point & pt)
  {
    return sample(pt, _r, _h, _concentration[N235]);
  }
  Real n238( const Point & pt)
  {
    return sample(pt, _r, _h, _concentration[N238]);
  }
  Real n239( const Point & pt)
  {
    return sample(pt, _r, _h, _concentration[N239]);
  }
  Real n240( const Point & pt)
  {
    return sample(pt, _r, _h, _concentration[N240]);
  }
  Real n241( const Point & pt)
  {
    return sample(pt, _r, _h, _concentration[N241]);
  }
  Real n242( const Point & pt)
  {
    return sample(pt, _r, _h, _concentration[N242]);
  }
  Real burnup( const Point & pt)
  {
    // The burnup is computed in units of MWd/kgU.  We want fission/atom-U.
    //  MWd | 1e6 Wd | 86400 Ws |  J |     MeV      | fission | .238 kgU |     mol U
    // ---------------------------------------------------------------------------------- = 0.00104
    //  kgU |  MWd   |   Wd     | Ws | 1.6022e-13 J | 205 MeV |  mol U   | 6.02e23 atom-U
    //
    // or
    //
    //  MWd | fission | .238 kgU |     mol U
    // ------------------------------------------ = MWd/kgU / MWd_per_fission * 3.952e-25
    //  kgU |  MWd   |  mol U   | 6.02e23 atom-U
    //
    // or
    //
    //  MWd | fission kgU
    // ------------------
    //  kgU |  MWd atom-U
    //
    return _fisskgU_per_MWdaU * sample(pt, _r, _h, _burnup);
  }
  Real burnupOld( const Point & pt)
  {
    return _fisskgU_per_MWdaU * sample(pt, _r, _h, _burnup_old);
  }
  Real rpf(const Point & pt)
  {
    return sample(pt, _r, _h, _rpf);
  }
  Real fissionRate(const Point & pt)
  {
    return sample(pt, _r, _h, _fission_rate);
  }
  Real fissionRateOld(const Point & pt)
  {
    return sample(pt, _r, _h, _fission_rate_old);
  }

  friend InputParameters validParams<BurnupFunction>();

};


template<>
InputParameters validParams<BurnupFunction>();

#endif
