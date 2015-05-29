/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "RelativePermeabilityAux.h"

template<>
InputParameters validParams<RelativePermeabilityAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("relative_permeability_uo", "Name of the User Object defining the relative permeability function");
  params.addRequiredCoupledVar("liquid_saturation_variable", "The liquid saturation variable.");
  MooseEnum fluid_phase_enum("liquid gas");
  params.addRequiredParam<MooseEnum>("fluid_phase_enum", fluid_phase_enum, "The fluid phase that this auxillary kernel is to act on (liquid or gas)");
  return params;
}

RelativePermeabilityAux::RelativePermeabilityAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _relative_permeabilityUO(getUserObject<RelativePermeability>("relative_permeability_uo")),
    _liquid_saturation(coupledValue("liquid_saturation_variable")),
    fluid_phase_enum(getParam<MooseEnum>("fluid_phase_enum"))
{}

Real
RelativePermeabilityAux::computeValue()
{
  Real relperm;

  if (fluid_phase_enum == "liquid") {
     relperm =  _relative_permeabilityUO.relativePermLiq(_liquid_saturation[_qp]);
  }
  if (fluid_phase_enum == "gas") {
     relperm =  _relative_permeabilityUO.relativePermGas(_liquid_saturation[_qp]);
  }

  return relperm;
}
