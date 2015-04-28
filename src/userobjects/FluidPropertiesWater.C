/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties of fresh water.                             */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidPropertiesWater.h"

template<>
InputParameters validParams<FluidPropertiesWater>()
{
  InputParameters params = validParams<FluidProperties>();
  params.addClassDescription("Thermophysical fluid properties of water.");
  return params;
}

FluidPropertiesWater::FluidPropertiesWater(const std::string & name, InputParameters parameters) :
  FluidProperties(name, parameters)
{
  // Reference critical constants used in to calculate thermophysical properties of water.
  _t_critical = 647.3;
  _p_critical = 22.064e6;
  _rho_critical = 322;
  _v_critical = 1.0 / _rho_critical;
  _t_c2k = 273.15;
  _R = 0.461526;
}

/// Density of water
/****************************************************************/
/* From IAPWS IF97 Revised Release on the IAPWS Industrial      */
/* Formulation 1997 for the Thermodynamic Properties of Water   */
/* and Steam.                                                   */
/*                                                              */
/* Valid for 273.15 K <= T <= 1073.15 K, p <= 100 MPa           */
/*          1073.15 K <= T <= 2273.15 K, p <= 50 Mpa            */
/*                                                              */
/* The calculation is split into five regions.                  */
/****************************************************************/
Real
  FluidPropertiesWater::fluidDensity(Real pressure, Real temperature) const

{


  return 0.;
}

/// Viscosity of water
/****************************************************************/
/* Eq. (10) from Release on the IAPWS Formulation 2008 for the  */
/* Viscosity of Ordinary Water Substance.                       */
/*                                                              */
/* Function takes temperature (in C) and density (kg/m^3), and  */
/* gives viscosity in Pa.s                                      */
/*                                                              */
/****************************************************************/
Real
  FluidPropertiesWater::fluidViscosity(Real temperature, Real density) const

{
  int ivs[21] = {0,1,2,3,0,1,2,3,5,0,1,2,3,4,0,1,0,3,4,3,5};
  int jvs[21] = {0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,4,4,5,6,6};

  Real h0v[4] = {1.67752, 2.20462, 0.6366564, -0.241605};
  Real h1v[21] = {5.20094e-1, 8.50895e-2, -1.08374, -2.89555e-1, 2.22531e-1, 9.99115e-1,
           1.88797, 1.26613, 1.20573e-1, -2.81378e-1, -9.06851e-1, -7.72479e-1,
           -4.89837e-1, -2.57040e-1, 1.61913e-1, 2.57399e-1, -3.25372e-2, 6.98452e-2,
           8.72102e-3, -4.35673e-3, -5.93264e-4};
  Real t0[4], t1[6], d1[7];

  Real mu_star = 1.e-6;

  Real tbar = (temperature + _t_c2k) / _t_critical;
  Real rhobar = density / _rho_critical;

  t0[0] = 1.;
  t0[1] = 1. / tbar;
  t0[2] = t0[1] * t0[1];
  t0[3] = t0[2] * t0[1];

  t1[0] = 1.;
  t1[1] = 1. / tbar - 1.;
  t1[2] = t1[1] * t1[1];
  t1[3] = t1[2] * t1[1];
  t1[4] = t1[3] * t1[1];
  t1[5] = t1[4] * t1[1];

  d1[0] = 1.;
  d1[1] = rhobar - 1.;
  d1[2] = d1[1] * d1[1]; 
  d1[3] = d1[2] * d1[1]; 
  d1[4] = d1[3] * d1[1]; 
  d1[5] = d1[4] * d1[1]; 
  d1[6] = d1[5] * d1[1]; 

  // Calculate mu0
  Real sum0 = 0.;
  for (int i = 0; i<= 3; i++) 
     sum0 += h0v[i] * t0[i];

  Real mu0 = 100. * std::sqrt(tbar) / sum0;

  // Now calculate mu1
  Real sum1 = 0.;
  for (int i = 0; i <= 20; i++)
     sum1 += t1[ivs[i]] * h1v[i] * d1[jvs[i]];

  Real mu1 = std::exp(rhobar * sum1);

  // The water viscosity (in Pa.s) is then given by
  
  return mu_star * mu0 * mu1;
}

/// Henry coefficient of CO2 as a function of temperature.
/*
Real
  FluidPropertiesWater::fluidHenryCoefficient(Real temperature) const

{

  Real co2henry = 200.0e+6; // Henry coefficient at T = 50C

  return co2henry;
}*/

/// Saturation pressure as a function of temperature.
/****************************************************************/
/* Eq. (30) from Revised Release on the IAPWS Industrial        */
/* Formulation 1997 for the Thermodynamic Properties of Water   */
/* and Steam, IAPWS 2007.                                       */
/*                                                              */
/* Function takes temperature (in C) and gives saturation       */
/* pressure in Pa.                                              */
/*                                                              */
/* Valid for 273.15 K <= t <= 647.096 K                         */
/****************************************************************/
Real
  FluidPropertiesWater::pSat(Real temperature) const
{
  Real tk = temperature + _t_c2k;

  Real n[10] = {0.11670521452767e4, -0.72421316703206e6, -0.17073846940092e2, 0.12020824702470e5,
                -0.32325550322333e7, 0.14915108613530e2, -0.48232657361591e4, 0.40511340542057e6,
                -0.238555575678490, 0.65017534844798e3};

  Real theta, theta2, a, b, c, p;

  // Check whether the input temperature is within the region of validity of this equation.
  if (tk >= _t_c2k && tk <= _t_critical) {
    theta = tk + n[8] / (tk - n[9]);
    theta2 = theta * theta;
    a = theta2 + n[0] * theta + n[1];
    b = n[2] * theta2 + n[3] * theta + n[4];
    c = n[5] * theta2 + n[6] * theta + n[7];
    p = std::pow(2.0 * c/(-b + std::sqrt(b * b - 4.0 * a * c)), 4.0);
  }
  else
    mooseError("FluidPropertiesWater::pSat: Temperature is outside range 0 <= t <= 400.964");

  return p * 1.e6;
}

/// Saturation temperature as a function of pressure.
/****************************************************************/
/* Eq. (31) from Revised Release on the IAPWS Industrial        */
/* Formulation 1997 for the Thermodynamic Properties of Water   */
/* and Steam, IAPWS 2007.                                       */
/*                                                              */
/* Function takes pressure (in Pa) and gives saturation         */
/* temperature in C.                                            */
/*                                                              */
/* Valid for 611.213 Pa <= p <= 22.064 MPa                      */
/****************************************************************/
Real
  FluidPropertiesWater::tSat(Real pressure) const
{
  Real n[10] = {0.11670521452767e4, -0.72421316703206e6, -0.17073846940092e2, 0.12020824702470e5,
                -0.32325550322333e7, 0.14915108613530e2, -0.48232657361591e4, 0.40511340542057e6,
                -0.238555575678490, 0.65017534844798e3};

  Real beta, beta2, e, f, g, d, t;

  // Check whether the input pressure is within the region of validity of this equation.
  if (pressure >= 611.23 && pressure <= _p_critical) {
    beta = std::pow(pressure / 1.e6, 0.25);
    beta2 = beta * beta;
    e = beta2 + n[2] * beta + n[5];
    f = n[0] * beta2 + n[3] * beta + n[6];
    g = n[1] * beta2 + n[4] * beta + n[7];
    d = 2.0 * g /(-f - std::sqrt(f * f - 4.0 * e * g));
    t = (n[9] + d - std::sqrt((n[9] + d) * (n[9] + d) - 4.0 * (n[8] + n[9] * d))) / 2.0;
  }
  else
    mooseError("FluidPropertiesWater::tSat: Pressure is outside range 611.213 Pa <= p <= 22.064 MPa");

  return t - _t_c2k;
}

/// Auxillary equation for the boundary between regions 2 and 3.
/****************************************************************/
/* Eq. (5) from Revised Release on the IAPWS Industrial         */
/* Formulation 1997 for the Thermodynamic Properties of Water   */
/* and Steam, IAPWS 2007.                                       */
/*                                                              */
/* Function takes temperature (in C) and gives pressure in Pa   */
/****************************************************************/
Real
  FluidPropertiesWater::b23p(Real temperature) const
{
  Real n[5] = {0.34805185628969e3, -0.11671859879975e1, 0.10192970039326e-2, 0.57254459862746e3,
               0.13918839778870e2};  

  Real tk = temperature + _t_c2k;

  return (n[0] + n[1] * tk + n[2] * tk * tk) * 1.e6;
} 

/// Auxillary equation for the boundary between regions 2 and 3.
/****************************************************************/
/* Eq. (6) from Revised Release on the IAPWS Industrial         */
/* Formulation 1997 for the Thermodynamic Properties of Water   */
/* and Steam, IAPWS 2007.                                       */
/*                                                              */
/* Function takes pressure (in Pa) and gives temperature in C   */
/****************************************************************/
Real
  FluidPropertiesWater::b23t(Real pressure) const
{
  Real n[5] = {0.34805185628969e3, -0.11671859879975e1, 0.10192970039326e-2, 0.57254459862746e3,
               0.13918839778870e2};  

  return n[3] + std::sqrt((pressure / 1.e6 - n[4]) / n[2]) - _t_c2k;
} 

/// Density function for Region 1 - single phase liquid region
/****************************************************************/
/* From Eq. (7) From Revised Release on the IAPWS Industrial    */
/* Formulation 1997 for the Thermodynamic Properties of Water   */
/* and Steam, IAPWS 2007.                                       */
/*                                                              */
/* Function takes pressure (in Pa) and temperature in C         */
/****************************************************************/
Real
  FluidPropertiesWater::cowat(Real pressure, Real temperature) const
{
  Real p_star1 = 16.53e6;
  Real t_star1 = 1386.;
  Real tk = temperature + _t_c2k;

  Real n1[34] = {0.14632971213167e0, -0.84548187169114e0, -0.37563603672040e1, 0.33855169168385e1,
                -0.95791963387872e0, 0.15772038513228e0, -0.16616417199501e-1,0.81214629983568e-3,
                 0.28319080123804e-3, -0.60706301565874e-3, -0.18990068218419e-1,-0.32529748770505e-1,
                -0.21841717175414e-1, -0.52838357969930e-4, -0.47184321073267e-3, -0.30001780793026e-3,
                 0.47661393906987e-4, -0.44141845330846e-5, -0.72694996297594e-15,-0.31679644845054e-4,
                -0.28270797985312e-5, -0.85205128120103e-9, -0.22425281908000e-5,-0.65171222895601e-6,
                -0.14341729937924e-12, -0.40516996860117e-6, -0.12734301741641e-8, -0.17424871230634e-9,
                -0.68762131295531e-18, 0.14478307828521e-19, 0.26335781662795e-22,-0.11947622640071e-22,
                 0.18228094581404e-23, -0.93537087292458e-25};

  int I1[34] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 8, 8, 21,
                  23, 29, 30, 31, 32};
  int J1[34] = {-2, -1, 0, 1, 2, 3, 4, 5, -9, -7, -1, 0, 1, 3, -3, 0, 1, 3, 17, -4, 0, 6, -5, -2, 10, -8,
                  -11, -6, -29, -31, -38, -39, -40, -41};

  // Now evaluate the sums
  Real sum1 = 0.;

  for (int i = 0; i <= 33; i++) {
    sum1 -= n1[i] * I1[i] * std::pow(7.1 - pressure / p_star1, I1[i]-1) * std::pow(t_star1 / tk - 1.222, J1[i]);
 }
  // The density of water in this region is then given by
  return p_star1 / (sum1 * _R * tk) / 1000.0;
}








/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/* Old IFC 1967 formulation

/// Density of water
Real
  FluidPropertiesWater::fluidDensity(Real pressure, Real temperature) const

{
  Real aa[23] = {6.824687741e3,-5.422063673e2,-2.096666205e4, 3.941286787e4, -6.733277739e4,
                 9.902381028e4,-1.093911774e5, 8.590841667e4, -4.511168742e4, 1.418138926e4,
                 -2.017271113e3, 7.982692717, -2.616571843e-2, 1.522411790e-3, 2.284279054e-2,
                 2.421647003e2, 1.269716088e-10, 2.074838328e-7, 2.174020350e-8, 1.105710498e-9,
                 1.293441934e1, 1.308119072e-5, 6.047626338e-14}; 
  Real sa[12] = {8.438375405e-1, 5.362162162e-4, 1.72, 7.342278489e-2, 4.975858870e-2,
                 6.537154300e-1, 1.15e-6, 1.5108e-5, 1.4188e-1, 7.002753165,
                 2.995284926e-4, 2.04e-1};

//Real t_critical = 647.3;
//Real p_critical = 2.212e7;
//Real t_c2k = 273.15;
//Real v_critical = 0.00317;

  Real tkr = (temperature + _t_c2k) / _t_critical;
  Real tkr2 = tkr * tkr;
  Real tkr18 = std::pow(tkr, 18.);
  Real tkr20 = tkr18 * tkr2;

  Real pnmr = pressure / _p_critical;
  Real pnmr2 = pnmr * pnmr;
  Real pnmr4 = pnmr2 * pnmr2;

  Real y = 1.0 - sa[0] * tkr2 - sa[1] / (std::pow(tkr, 6.));
  Real x = sa[2] * y * y - 2.0 * (sa[3] * tkr - sa[4] * pnmr);

  // Note: z may become negative in the vicinity of the critical point. 
  if (x < 0.0 ) 
    mooseError("FluidPropertiesWater::fluidDensity: negative density\n");
  else 
    x = std::sqrt(x); 

  Real z = y + x;
  Real u1 = aa[11] * sa[4] * std::pow(z, -5./17.);
  Real u2 = 1.0 / (sa[7] + std::pow(tkr, 11.));
  Real u3 = aa[17] + (2.0 * aa[18] + 3.0 * aa[19] * pnmr) * pnmr;
  Real u4 = 1.0 / (sa[6] + tkr18 * tkr);
  Real u5 = std::pow(sa[9] + pnmr, -4.);
  Real u6 = sa[10] - 3.0 * u5;
  Real u7 = aa[19] * tkr18 * (sa[8] + tkr2);
  Real u8 = aa[14] * std::pow(sa[5] - tkr, 9.);

  Real vr = u1 + aa[12] + tkr * (aa[13] + aa[14] * tkr) + u8 * (sa[5] - tkr) +
            aa[16] * u4 - u2 * u3 - u6 * u7 + (3.0 * aa[21] * (sa[11] - tkr) +
            4.0 * aa[22] * pnmr / tkr20) * pnmr2;

  return 1.0 / (vr * _v_critical);

// Hard code constant bulk modulus pressure dependence.
//Real waterdensity = 996.53 * std::exp(pressure /(2.2e9)); // Density at P = 20MPa, T = 50C
//
//return waterdensity;
}

*/
