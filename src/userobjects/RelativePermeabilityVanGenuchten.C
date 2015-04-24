/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* van Genuchten relative permeability for multiphase flow in   */
/* porous media.                                                */
/*                                                              */
/* Liquid relative permeability given by                        */ 
/* van Genuchten, M. Th., A closed for equation for             */
/* predicting the hydraulic conductivity of unsaturated soils,  */
/* Soil Sci. Soc., 44, 892-898 (1980). Gas relative             */
/* given by                                                     */
/* Corey, A. T., The interrelation between gas and oil          */
/* relative permeabilities, Prod. Month. 38-41 (1954).          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "RelativePermeabilityVanGenuchten.h"

template<>
InputParameters validParams<RelativePermeabilityVanGenuchten>()
{
  InputParameters params = validParams<RelativePermeability>();
  params.addRequiredRangeCheckedParam<Real>("sat_lr", "sat_lr >= 0 & sat_lr <= 1", "Liquid residual saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("sat_ls", "sat_ls >= 0 & sat_ls <= 1", "Liquid fully saturated saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("sat_gr", "sat_gr >= 0 & sat_gr <= 1", "Gas residual saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("m", "m > 0 & m < 1", "van Genuchten exponent m.");
  params.addClassDescription("van Genuchten form of relative permeability.");
  return params;
}

RelativePermeabilityVanGenuchten::RelativePermeabilityVanGenuchten(const std::string & name, InputParameters parameters) :
  RelativePermeability(name, parameters),
  _sat_lr(getParam<Real>("sat_lr")),
  _sat_ls(getParam<Real>("sat_ls")),
  _sat_gr(getParam<Real>("sat_gr")),
  _m(getParam<Real>("m"))
{
}

/// Define van Genuchten relative permeability and override relativePermLiq.
Real
RelativePermeabilityVanGenuchten::relativePermLiq(Real sat_liq) const
{
  
  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityVanGenuchten: Liquid saturation is outside the range 0 <= Sl <= 1\n");


  if (sat_liq >= _sat_ls) {
    return 1.0;
  }

  if (sat_liq <= _sat_lr) {
    return 0.0;
  }

  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real krel = std::pow(sat_eff, 0.5)*std::pow(1.0 - std::pow(1.0 - std::pow(sat_eff, 1.0/_m), _m), 2.0);

  // Bound just in case
  if (krel < 0) { krel = 0;}
  if (krel > 1) { krel = 1;}
  return krel;
}


/// Define van Genuchten relative permeability and override relativePermGas.
Real
RelativePermeabilityVanGenuchten::relativePermGas(Real sat_liq) const
{

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityVanGenuchten: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  if (sat_liq >= _sat_ls) {
    return 0.0;
  }

  if (sat_liq <= _sat_lr) {
    return 1.0;
  }
  
  if (_sat_gr > 0.0) {
    
  Real sat_eff = (sat_liq - _sat_lr)/(1.0 - _sat_lr - _sat_gr);
  Real krel = std::pow(1.0 - sat_eff, 2.0) * (1.0 - std::pow(sat_eff, 2.0));

  // Bound just in case
  if (krel < 0) { krel = 0;}
  if (krel > 1) { krel = 1;}

  return krel;
  }

  else {
  return 1.0 - RelativePermeabilityVanGenuchten::relativePermLiq(sat_liq);
  }

}
