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
  MooseEnum property_enum("relperm drelperm", "relperm");
  params.addRequiredParam<MooseEnum>("property_enum", property_enum, "The relative permeability property that this auxillary kernel is to act on");
  return params;
}

RelativePermeabilityAux::RelativePermeabilityAux(const InputParameters & parameters) :
    AuxKernel(parameters),

    _relative_permeabilityUO(getUserObject<RelativePermeability>("relative_permeability_uo")),
    _liquid_saturation(coupledValue("liquid_saturation_variable")),
    fluid_phase_enum(getParam<MooseEnum>("fluid_phase_enum")),
    property_enum(getParam<MooseEnum>("property_enum"))
{}

Real
RelativePermeabilityAux::computeValue()
{
  Real property = 0.;

  if (property_enum == "relperm")
  {
    if (fluid_phase_enum == "liquid")
      property =  _relative_permeabilityUO.relativePermLiquid(_liquid_saturation[_qp]);

    else if (fluid_phase_enum == "gas")
     property =  _relative_permeabilityUO.relativePermGas(_liquid_saturation[_qp]);
  }

  else if (property_enum == "drelperm")
  {
    if (fluid_phase_enum == "liquid")
      property =  _relative_permeabilityUO.dRelativePermLiquid(_liquid_saturation[_qp]);

    else if (fluid_phase_enum == "gas")
     property =  _relative_permeabilityUO.dRelativePermGas(_liquid_saturation[_qp]);
  }

  return property;
}
