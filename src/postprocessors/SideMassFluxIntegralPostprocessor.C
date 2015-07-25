/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "SideMassFluxIntegralPostprocessor.h"

template<>
InputParameters validParams<SideMassFluxIntegralPostprocessor>()
{
  InputParameters params = validParams<SideIntegralVariablePostprocessor>();
  params.addRequiredCoupledVar("fluid_density_variable", "The fluid density auxillary variable");
  params.addRequiredCoupledVar("fluid_viscosity_variable", "The fluid viscosity auxillary variable");
  params.addCoupledVar("mass_fraction_variable", 1.0, "The variable describing the mass fraction of the component in this phase");
  params.addCoupledVar("relative_permeability_variable", 1.0, "The relative permeability variable");
  return params;
}

SideMassFluxIntegralPostprocessor::SideMassFluxIntegralPostprocessor(const InputParameters & parameters) :
    SideIntegralVariablePostprocessor(parameters),
    _permeability(getMaterialProperty<RealTensorValue>("permeability")),
    _gravity(getMaterialProperty<RealVectorValue>("gravity")),
    _fluid_density(coupledValue("fluid_density_variable")),
    _fluid_viscosity(coupledValue("fluid_viscosity_variable")),
    _mass_fraction(coupledValue("mass_fraction_variable")),
    _relative_permeability(coupledValue("relative_permeability_variable"))
{}


Real
SideMassFluxIntegralPostprocessor::computeQpIntegral()
{
  RealTensorValue fluid_mobility = _relative_permeability[_qp] * _permeability[_qp] * _fluid_density[_qp] / _fluid_viscosity[_qp];

  RealVectorValue fluid_flux =  fluid_mobility * (_grad_u[_qp] -
                                 _fluid_density[_qp] * _gravity[_qp]);  // TODO: general form for multicomponent as well.
  return _mass_fraction[_qp] * fluid_flux * _normals[_qp];
}
