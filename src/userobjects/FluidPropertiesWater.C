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
}

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

  Real t_critical = 647.3;
  Real p_critical = 2.212e7;
  Real tc2k = 273.15;
  Real v_critical = 0.00317;

  Real tkr = (temperature + tc2k) / t_critical;
  Real tkr2 = tkr * tkr;
  Real tkr18 = std::pow(tkr, 18.);
  Real tkr20 = tkr18 * tkr2;

  Real pnmr = pressure / p_critical;
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

  return 1.0 / (vr * v_critical);

// Hard code constant bulk modulus pressure dependence.
//Real waterdensity = 996.53 * std::exp(pressure /(2.2e9)); // Density at P = 20MPa, T = 50C
//
//return waterdensity;
}

/// Viscosity of water
Real
  FluidPropertiesWater::fluidViscosity(Real pressure, Real temperature) const

{

  Real waterviscosity = 550.56e-6; // Viscosity at P = 20MPa, T = 50C

  return waterviscosity;
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
  Real t_critical = 674.096;
  Real t_c2k = 273.15;
  Real tk = temperature + t_c2k;

  Real n[10] = {0.11670521452767e4, -0.72421316703206e6, -0.17073846940092e2, 0.12020824702470e5,
                -0.32325550322333e7, 0.14915108613530e2, -0.48232657361591e4, 0.40511340542057e6,
                -0.238555575678490, 0.65017534844798e3};

  Real theta, theta2, a, b, c, p;

  // Check whether the input temperature is within the region of validity of this equation.
  if (tk >= t_c2k && tk <= t_critical) {
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
  if (pressure >= 611.23 && pressure <= 22.064e6) {
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

  return t - 273.15;
}
