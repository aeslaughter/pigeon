#include "BurnupFunction.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional>

template<>
InputParameters validParams<BurnupFunction>()
{
  InputParameters params = validParams<Function>();

  std::vector<Real> i_enrich_default(BurnupFunction::NUM_ISOTOPES);
  i_enrich_default[0] = 0.05;
  i_enrich_default[1] = 0.95;
  i_enrich_default[2] = 0.0;
  i_enrich_default[3] = 0.0;
  i_enrich_default[4] = 0.0;
  i_enrich_default[5] = 0.0;

  params.addParam<Real>("value", 1.0, "Default/scaling value.");
  params.addParam<bool>("rpf_active", true, "Flag for turning radial power factor on.");
  params.addParam<unsigned>("num_radial", 80, "Number of radial grid points.");
  params.addParam<unsigned>("num_axial", 20, "Number of axial grid points.");
  params.addParam<unsigned>("axial_axis",1,"Coordinate axis of the axial direction of the fuel stack (0, 1, or 2 for x, y, or z");
  params.addRequiredParam<Real>("a_lower", "The lower axial coordinate.");
  params.addRequiredParam<Real>("a_upper", "The upper axial coordinate.");
  params.addParam<Real>("fuel_inner_radius", 0.0, "The inner radius.");
  params.addParam<Real>("fuel_outer_radius", 0.0041, "The outer radius.");
  params.addParam<Real>("bias", 1.0, "Bias for radial point spacing.  Must be between 0.5 and 2.0");
  params.addParam<Real>("fuel_volume_ratio", 1.0, "Reduction factor for deviation from right circular cylinder fuel.  The ratio of actual volume to right circular cylinder volume.");
  params.addRequiredParam<Real>("density", "The density.");
  // 205 MeV/fission * 1.6022e-13 J/MeV = 3.28451e-11 J/fission
  params.addParam<Real>("energy_per_fission", 3.28451e-11, "Energy released per fission (J/fission)");
  params.addParam<std::vector<Real> >("i_enrich", i_enrich_default, "The initial enrichment for the six isotopes.");
  params.addPrivateParam<std::vector<Real> >("sigma_a");
  params.addParam<std::vector<Real> >("sigma_c", "The capture cross sections for the six isotopes.");
  params.addParam<std::vector<Real> >("sigma_f", "The fission cross sections for the six isotopes.");
  params.addParam<std::vector<Real> >("sigma_a_thermal", "The absorption (thermal) cross sections for the six isotopes.");
  params.addParam<FunctionName>("rod_ave_lin_pow", "", "The power function");
  params.addParam<FunctionName>("axial_power_profile", "", "The axial power profile function");

  params.addParam<Real>("p1", "Distribution function coefficient p1.  If not given, will take default value based on reactor_type");

  MooseEnum reactor_type("LWR HWR", "LWR");
  params.addParam<MooseEnum>("reactor_type", reactor_type, "Reactor type.  Choices are " + reactor_type.getRawNames());

  // Hide BurnupFunction from the input file dump
  params.addPrivateParam<std::string>("built_by_action", "");
  return params;
}

//
// This routine is based on:
// The radial distribution of plutonium in high burnup UO2 fuels,
//     K. Lassmann et. al, JNM 208 (1994), p. 223-231.
// The ideas are coded in TUBRNP by Lassmann.
//

BurnupFunction::BurnupFunction(const std::string & name, InputParameters params) :
  Function( name, params ),
  FunctionInterface( params ),
  _ralp(getParam<FunctionName>("rod_ave_lin_pow") != "" ?
        &getFunction("rod_ave_lin_pow") :
        NULL),
  _axial_profile(getParam<FunctionName>("axial_power_profile") != "" ?
                 &getFunction("axial_power_profile") :
                 NULL),
  _value(getParam<Real>( "value" )),
  _rpf_active(getParam<bool>( "rpf_active" )),
  _num_radial(getParam<unsigned>("num_radial")),
  _num_axial(getParam<unsigned>("num_axial")),
  _axial_axis(getParam<unsigned>("axial_axis")),
  _a_lower(getParam<Real>("a_lower")),
  _a_upper(getParam<Real>("a_upper")),
  _height(_a_upper-_a_lower),
  _r_inner(getParam<Real>("fuel_inner_radius")),
  _r_outer(getParam<Real>("fuel_outer_radius")),
  _bias(getParam<Real>("bias")),
  _fuel_volume_ratio(getParam<Real>("fuel_volume_ratio")),
  _time_last_requested(declareRestartableData<Real>("time_last_requested", -std::numeric_limits<Real>::max())),
  _density( getParam<Real>("density") ),
  _porosity_fraction( 0 ),
  // MeV/fission * 1.6022e-13 J/MeV = J/fission
  _energy_per_fission( getParam<Real>("energy_per_fission" ) ), // J/fission
  // Number of atoms per m^3 of fuel
  // 0.270 = kg UO2 per mole
  // 6.0221415e23 = Avagadro's number
  _ntot(( 1-_porosity_fraction ) * _density / 0.270 * 6.0221415e23 ),
  _r(_num_radial),
  _h(_num_axial),
  _volume_frac(_num_radial),
  _p1(p1()),
  _i_enrich(getParam<std::vector<Real> >("i_enrich")),
  _sigma_f(sigma_f()),
  _sigma_c(sigma_c()),
  _sigma_a(sigma_a("sigma_a")),
  _sigma_a_thermal(sigma_a("sigma_a_thermal")),
  // Concentrations are atoms/m^3 of UO2
  // Index [i][j][k] -> [isotope][height][radius]
  _concentration(
    declareRestartableData<std::vector<std::vector<std::vector<Real> > > >("concentration",
      std::vector<std::vector<std::vector<Real> > >(NUM_ISOTOPES,
        std::vector<std::vector<Real> >(_num_axial, std::vector<Real>(_num_radial, 0.0))))),
  _concentration_old(NUM_ISOTOPES, std::vector<std::vector<Real> >(_num_axial, std::vector<Real>(_num_radial, 0.0))),
  // Index [i][j] -> [isotope][height]
  _average_concentration(
    declareRestartableData<std::vector<std::vector<Real> > >("average_concentration",
      std::vector<std::vector<Real> >(NUM_ISOTOPES, std::vector<Real>(_num_axial, 0.0)))),
  _average_concentration_old(
    declareRestartableData<std::vector<std::vector<Real> > >("average_concentration_old",
      std::vector<std::vector<Real> >(NUM_ISOTOPES, std::vector<Real>(_num_axial, 0.0)))),
  // Index [j][k] -> [height][radius]
  _burnup(
    declareRestartableData<std::vector<std::vector<Real> > >("burnup",
      std::vector<std::vector<Real> >(_num_axial, std::vector<Real>(_num_radial, 0.0)))), // MWd/kgU
  _burnup_old(_num_axial, std::vector<Real>(_num_radial, 0.0)),
  _fission_rate(_num_axial, std::vector<Real>(_num_radial, 0.0)),
  _fission_rate_old(_num_axial, std::vector<Real>(_num_radial, 0.0)),
  _rpf(
    declareRestartableData<std::vector<std::vector<Real> > >("RPF",
      std::vector<std::vector<Real> >(_num_axial, std::vector<Real>(_num_radial, 1.0)))),
  // Index [k] -> [radius]
  _f_normalized(_num_radial, 0.0),
  // 205 MeV | 1.6022e-13 J | W*s |   day   |   MW
  // ------------------------------------------------ = 3.8e-22 MWd/fission
  // fission |     MeV      |  J  | 86400 s |  1e6 W
  _MWdperFission( _energy_per_fission / 86400e6 ),
  _fisskgU_per_MWdaU( 86400e6 * .238 / (_energy_per_fission * 6.0221415e23 ) )

{
  if (_num_axial < 2)
  {
    mooseError("Number of axial positions must be >= 2.");
  }
  if (_bias < 0.5 || _bias > 2.0)
  {
    std::stringstream msg;
    msg << "Bias was given as " << _bias << ". "
        << "Allowed range: 0.5 <= bias <= 2.0";
    mooseError( msg );
  }
  if (_fuel_volume_ratio > 1)
  {
    mooseError("Burnup: fuel_volume_ratio must be <= 1");
  }

  if (_sigma_f.size() != _sigma_c.size() ||
      _sigma_c.size() != _sigma_a.size() ||
      _sigma_a.size() != _sigma_a_thermal.size() ||
      _sigma_a_thermal.size() != NUM_ISOTOPES)
  {
    std::stringstream msg;
    msg << "All cross section lists must have "
        << NUM_ISOTOPES
        << " entries.";
    mooseError(msg.str());
  }

  const Real h( _height / (_num_axial-1) );
  _h[0] = _a_lower;
  for (unsigned j(1); j < _num_axial; ++j)
  {
    _h[j] = _h[j-1] + h;
  }

  computeRadialLocations( _r_inner, _r_outer, _bias, _r );

  computeVolumeFractions( h, _num_axial, _r, _volume_frac );

  // Conversion from power to fission rate
  // p_to_fr => fission/J/m^2
  _p_to_fr = 1.0 / ( _energy_per_fission * _fuel_volume_ratio * M_PI * (_r[_num_radial-1]*_r[_num_radial-1] - _r[0]*_r[0]) );


  mooseAssert(_i_enrich.size() == NUM_ISOTOPES, "Initial enrichment vector has wrong size");
  std::vector<Real> i_concen(_i_enrich);
  std::transform(i_concen.begin(), i_concen.end(), i_concen.begin(),
                 std::bind1st(std::multiplies<Real>(), _ntot));


  for (unsigned int i(0); i < NUM_ISOTOPES; ++i)
  {
    for (unsigned int j(0); j < _num_axial; ++j)
    {
      _average_concentration[i][j] = _average_concentration_old[i][j] = i_concen[i];
      for (unsigned int k(0); k < _num_radial; ++k)
      {
        _concentration[i][j][k] = _concentration_old[i][j][k] = i_concen[i];
      }
    }
  }

  // p1 is 3.45 for LWR, 2.21 for Halden HWR (Handbook of Nuclear Engr, Vol. 2,
  //   also FRAPCON manual).
  // p2 is 3    for LWR and HWR
  // p3 is 0.45 for LWR and HWR
  const Real p2(3);
  const Real p3(0.45);
  Real average_f(0);
  for (unsigned int k(0); k < _num_radial; ++k)
  {
    // The equation for f requires length in millimeters
    // Multiply by 1000 to get millimeters from meters
    _f_normalized[k] = 1 + _p1 * std::exp(-p2*std::pow((_r[_num_radial-1] - _r[k])*1000, p3));
    average_f += _f_normalized[k] * _volume_frac[k];
  }
  for (unsigned int k(0); k < _num_radial; ++k)
  {
    _f_normalized[k] /= average_f;
  }
}

Real
BurnupFunction::p1() const
{
  Real p1 = 3.45;
  if (isParamValid("p1"))
  {
    p1 = getParam<Real>("p1");
  }
  else if (getParam<MooseEnum>("reactor_type") == "LWR")
  {
    // Default value 3.45
  }
  else if (getParam<MooseEnum>("reactor_type") == "HWR")
  {
    p1 = 2.21;
  }
  return p1;
}

std::vector<Real>
BurnupFunction::sigma_f() const
{
  std::vector<Real> sigma_f(BurnupFunction::NUM_ISOTOPES); // fission
  sigma_f[0] = 41.5;
  sigma_f[1] = 0;
  sigma_f[2] = 105.0;
  sigma_f[3] = 0.584;
  sigma_f[4] = 120.0;
  sigma_f[5] = 0.458;
  if (isParamValid("sigma_f"))
  {
    sigma_f = getParam<std::vector<Real> >("sigma_f");
  }
  else if (getParam<MooseEnum>("reactor_type") == "LWR")
  {
    // Default value
  }
  else if (getParam<MooseEnum>("reactor_type") == "HWR")
  {
    sigma_f[0] = 107.95;
    sigma_f[1] = 0;
    sigma_f[2] = 239.18;
    sigma_f[3] = 0.304;
    sigma_f[4] = 296.95;
    sigma_f[5] = 0.191;
  }
  return sigma_f;
}

std::vector<Real>
BurnupFunction::sigma_c() const
{
  std::vector<Real> sigma_c(BurnupFunction::NUM_ISOTOPES);
  sigma_c[0] = 9.7;
  sigma_c[1] = 0.78;
  sigma_c[2] = 58.6;
  sigma_c[3] = 100.0;
  sigma_c[4] = 50.0;
  sigma_c[5] = 80.0;
  if (isParamValid("sigma_c"))
  {
    sigma_c = getParam<std::vector<Real> >("sigma_c");
  }
  else if (getParam<MooseEnum>("reactor_type") == "LWR")
  {
    // Default value
  }
  else if (getParam<MooseEnum>("reactor_type") == "HWR")
  {
    sigma_c[0] = 22.3;
    sigma_c[1] = 1.16;
    sigma_c[2] = 125.36;
    sigma_c[3] = 127.26;
    sigma_c[4] = 122.41;
    sigma_c[5] = 91.3;
  }
  return sigma_c;
}

std::vector<Real>
BurnupFunction::sigma_a(const std::string & name) const
{
  std::vector<Real> sigma_a(BurnupFunction::NUM_ISOTOPES);
  if (isParamValid(name))
  {
    sigma_a = getParam<std::vector<Real> >(name);
  }
  else
  {
    for (unsigned i(0); i < BurnupFunction::NUM_ISOTOPES; ++i)
    {
      sigma_a[i] = _sigma_f[i] + _sigma_c[i];
    }
  }
  return sigma_a;
}



void
BurnupFunction::setup( const Real delta_t,
                       const Real time )
{

  if (time == _time_last_requested)
  {
    // The data has already been computed and is ready for interpolation.
    return;
  }

  // Check if we need to update state variables.  If the solve has been cut back,
  // we must not update them.
  if (time > _time_last_requested)
  {
    // Update state variables (save new into old)
    _concentration.swap( _concentration_old );
    _average_concentration.swap( _average_concentration_old );
    _burnup.swap( _burnup_old );
    _fission_rate.swap( _fission_rate_old );
  }
  _time_last_requested = time;


  computeFissionRate( time );


  computeBurnup( delta_t );


  if (_rpf_active)
  {
    computeRadialPowerFactor();
  }

  // Dump data to screen
//   Moose::out << std::setprecision(8)
//              << std::setw(11) << time
//              << "\t" << std::setw(11) << _concentration[0][0][_num_radial-1]
//              << "\t" << std::setw(11) << _concentration[1][0][_num_radial-1]
//              << "\t" << std::setw(11) << _concentration[2][0][_num_radial-1]
//              << "\t" << std::setw(11) << _concentration[3][0][_num_radial-1]
//              << "\t" << std::setw(11) << _concentration[4][0][_num_radial-1]
//              << "\t" << std::setw(11) << _concentration[5][0][_num_radial-1]
//              << "\t" << std::setw(11) << _fission_rate[0][_num_radial-1]
//              << "\t" << std::setw(11) << _burnup[0][_num_radial-1]
//              << std::endl;
}

void
BurnupFunction::computeBurnup( const Real delta_t )
{

  for (unsigned int j(0); j < _num_axial; ++j)
  {

    for (unsigned int k(0); k < _num_radial; ++k)
    {
      _burnup[j][k] = _burnup_old[j][k] + delta_t * _fission_rate[j][k] * _MWdperFission / _density * 270.0/238.0;
    }

  }
}

void
BurnupFunction::computeFissionRate( const Real time )
{

  if (!_ralp)
  {
    for (unsigned int j(0); j < _num_axial; ++j)
    {

      Real value(_value);
      if ( _axial_profile )
      {
        Point p(0, _h[j], 0);
        value *= _axial_profile->value( time, p );
      }
      if (value < 0)
      {
        mooseError("Negative fission rate in BurnupFunction");
      }

      for (unsigned int k(0); k < _num_radial; ++k)
      {
        _fission_rate[j][k] = value;
      }
    }

  }
  else
  {
    for (unsigned int j(0); j < _num_axial; ++j)
    {

      Point p(0, _h[j], 0);
      mooseAssert(_ralp, "Rod ave lin power function not defined.");
      Real ralp = _value * _ralp->value( time, p );
      if ( _axial_profile )
      {
        ralp *= _axial_profile->value( time, p );
      }
      if (ralp < 0)
      {
        mooseError("Negative fission rate in BurnupFunction");
      }

      for (unsigned int k(0); k < _num_radial; ++k)
      {
        // _p_to_fr => fission/J/m^2
        _fission_rate[j][k] = ralp * _p_to_fr * _rpf[j][k];
      }

    }
  }
}

void
BurnupFunction::computeRadialPowerFactor()
{

  for (unsigned int j(0); j < _num_axial; ++j)
  {

    const Real f2( 0.8815 );  // 238 / 270
    const Real phi( _density /
                      ( _MWdperFission *
                        ( _average_concentration[N235][j]*_sigma_f[N235] +
                          _average_concentration[N239][j]*_sigma_f[N239] +
                          _average_concentration[N241][j]*_sigma_f[N241] ) ) * f2
                      );

    for (unsigned int k(0); k < _num_radial; ++k)
    {
      // Burnup is in MWd/kgU
      const Real conversion_factor( phi * ( _burnup[j][k] - _burnup_old[j][k] ) );
      _concentration[N235][j][k] = _concentration_old[N235][j][k] - conversion_factor * ( _sigma_a[N235]*_concentration_old[N235][j][k]);
      _concentration[N238][j][k] = _concentration_old[N238][j][k] - conversion_factor * (                                                 _sigma_a[N238]*_average_concentration_old[N238][j] * _f_normalized[k]);
      _concentration[N239][j][k] = _concentration_old[N239][j][k] + conversion_factor * (-_sigma_a[N239]*_concentration_old[N239][j][k] + _sigma_c[N238]*_average_concentration_old[N238][j] * _f_normalized[k]);
      _concentration[N240][j][k] = _concentration_old[N240][j][k] + conversion_factor * (-_sigma_a[N240]*_concentration_old[N240][j][k] + _sigma_c[N239]*_concentration_old[N239][j][k]);
      _concentration[N241][j][k] = _concentration_old[N241][j][k] + conversion_factor * (-_sigma_a[N241]*_concentration_old[N241][j][k] + _sigma_c[N240]*_concentration_old[N240][j][k]);
      _concentration[N242][j][k] = _concentration_old[N242][j][k] + conversion_factor * (-_sigma_a[N242]*_concentration_old[N242][j][k] + _sigma_c[N241]*_concentration_old[N241][j][k]);

      _concentration[N235][j][k] = std::max(_concentration[N235][j][k], 0.0);
      _concentration[N238][j][k] = std::max(_concentration[N238][j][k], 0.0);
      _concentration[N239][j][k] = std::max(_concentration[N239][j][k], 0.0);
      _concentration[N240][j][k] = std::max(_concentration[N240][j][k], 0.0);
      _concentration[N241][j][k] = std::max(_concentration[N241][j][k], 0.0);
      _concentration[N242][j][k] = std::max(_concentration[N242][j][k], 0.0);

    }

    // Compute average concentrations
    for (unsigned int i(0); i < NUM_ISOTOPES; ++i)
    {
      _average_concentration[i][j] = 0;
      for (unsigned int k(0); k < _num_radial; ++k)
      {
        _average_concentration[i][j] += _concentration[i][j][k] * _volume_frac[k];
      }
    }

    // Compute RPF
    Real saTot(0);
    for (unsigned int i(0); i < NUM_ISOTOPES; ++i)
    {
      saTot += _sigma_a_thermal[i] * _average_concentration[i][j];
    }
    // 1e-28 => 1e-24cm^2/barn * 1m^2/(100cm)^2
    const Real oneem28( 1e-28 );
    saTot *= oneem28;
    const Real paramsstot(300);
    const Real sstot( paramsstot * _ntot * oneem28 );
    const Real diff( 1/(3*sstot) );
    const Real kappav( std::sqrt(saTot/diff) );
    Real average_rpf(0);
    Real hollow_factor(0);
    if (_r[0] > 0)
    {
      hollow_factor = I1(kappav*_r[0])/K1(kappav*_r[0]);
    }
    for (unsigned int k(0); k < _num_radial; ++k)
    {
      Real formf(0);
      if (_r[0] == 0)
      {
        // Solid cylinder
        formf = I0( kappav * _r[k] );
      }
      else
      {
        // Hollow cylinder
        formf = I0( kappav * _r[k] ) + hollow_factor*K0(kappav*_r[k]);
      }
      Real sftot(0);
      for (unsigned int i(0); i < NUM_ISOTOPES; ++i)
      {
        sftot += _sigma_f[i]*_concentration[i][j][k];
      }
      _rpf[j][k] = formf * sftot;
      average_rpf += _rpf[j][k] * _volume_frac[k];
    }
    for (unsigned int k(0); k < _num_radial; ++k)
    {
      _rpf[j][k] /= average_rpf;
    }
  }
}

Real
BurnupFunction::sample( const Point & pt,
                        const std::vector<Real> & radius,
                        const std::vector<Real> & height,
                        const std::vector<std::vector<Real> > & data ) // [height][radius]
{
  unsigned r1 = 0;
  unsigned r2 = 2;
  if (_axial_axis == 0)
  {
    r1 = 1;
  }
  else if (_axial_axis == 2)
  {
    r2 = 1;
  }
  else if (_axial_axis > 2)
  {
    mooseError("axial_axis must be 0, 1, or 2");
  }

  Real h = pt(_axial_axis);
  Real r = std::sqrt( pt(r1)*pt(r1) + pt(r2)*pt(r2) );

  unsigned int ax_lower, ax_upper;
  bounds( h, height, ax_lower, ax_upper );
  unsigned int r_lower, r_upper;
  bounds( r, radius, r_lower, r_upper );

  mooseAssert( ax_upper < height.size(), "Out-of-bounds index for height." );
  mooseAssert( r_upper < radius.size(), "Out-of-bounds index for radius." );

  if (ax_lower == ax_upper && r_lower == r_upper)
  {
    return data[ax_lower][r_lower];
  }
  const Real rl(radius[r_lower]);
  const Real ru(radius[r_upper]);
  const Real dll(data[ax_lower][r_lower]);
  const Real dlu(data[ax_lower][r_upper]);
  if (ax_lower == ax_upper)
  {
    return dll + (dlu-dll)*(r-rl)/(ru-rl);
  }
  const Real hl(height[ax_lower]);
  const Real hu(height[ax_upper]);
  const Real dul(data[ax_upper][r_lower]);
  if (r_lower == r_upper)
  {
    return dll + (dul-dll)*(h-hl)/(hu-hl);
  }
  const Real duu(data[ax_upper][r_upper]);
  return (dll*(hu-h)*(ru-r)+dlu*(hu-h)*(r-rl)+duu*(h-hl)*(r-rl)+dul*(h-hl)*(ru-r))/((hu-hl)*(ru-rl));
}

Real
BurnupFunction::value(Real, const Point &)
{
  mooseError("BurnupFunction::value is not implemented.  Call 'burnup', 'fissionRate', 'rpf', or one of the concentration retrieval methods.");
  return 0;
}
