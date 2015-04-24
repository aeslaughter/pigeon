/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* van Genuchten capillary pressure for multiphase flow in      */
/* porous media.                                                */
/*                                                              */
/* Based on                                                     */
/* van Genuchten, M. Th., A closed for equation for             */
/* predicting the hydraulic conductivity of unsaturated soils,  */
/* Soil Sci. Soc., 44, 892-898 (1980).                          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "CapillaryPressureVanGenuchten.h"

template<>
InputParameters validParams<CapillaryPressureVanGenuchten>()
{
  InputParameters params = validParams<CapillaryPressure>();
  params.addRequiredRangeCheckedParam<Real>("sat_lr", "sat_lr >= 0 & sat_lr <= 1", "Liquid residual saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("sat_ls", "sat_ls >= 0 & sat_ls <= 1", "Liquid fully saturated saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("m", "m >= 0 & m <= 1", "van Genuchten exponent m.");
  params.addRequiredRangeCheckedParam<Real>("cp_max", "cp_max > 0", "Maximum capillary pressure");
  params.addRequiredRangeCheckedParam<Real>("p0", "p0 > 0", "P0");
  params.addClassDescription("van Genuchten form of capillary pressure.");
  return params;
}

CapillaryPressureVanGenuchten::CapillaryPressureVanGenuchten(const std::string & name, InputParameters parameters) :
  CapillaryPressure(name, parameters),
  _sat_lr(getParam<Real>("sat_lr")),
  _sat_ls(getParam<Real>("sat_ls")),
  _m(getParam<Real>("m")),
  _cp_max(getParam<Real>("cp_max")),
  _p0(getParam<Real>("p0"))
{
}

/// Define van Genuchten capillary pressure and override capillary_pressure.
Real
CapillaryPressureVanGenuchten::capillaryPressure(Real sat_liq) const
{

  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real cp;

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0) 
    mooseError("CapillaryPressureVanGenuchten: Liquid saturation is negative\n");
  if (sat_liq > 1.0) 
    mooseError("CapillaryPressureVanGenuchten: Liquid saturation is greater than 1.0\n");

  if (sat_eff > 1.0) 
     { sat_eff = 1.0; } // Restrict sat_eff to be less than or equal to unity

  if (sat_liq <= _sat_lr)
     { cp = - _cp_max; } // If sat_eff <= _sat_lr, Pc = cp_max.

  else // else use Van Genuchten formulation
     { cp = -1.0 * _p0 * std::pow(std::pow(sat_eff,-1.0/_m) - 1.0, 1.0 - _m); }

  // Constrain the capillary pressure cp so that - _cp_max <= cp <= 0
  if (cp < -_cp_max) { cp = -_cp_max;}
  if (cp > 0.0) { cp = 0.0;}

  return cp; 
}
