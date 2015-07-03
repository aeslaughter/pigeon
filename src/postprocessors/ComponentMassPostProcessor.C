/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "ComponentMassPostprocessor.h"

template<>
InputParameters validParams<ComponentMassPostprocessor>()
{
  InputParameters params = validParams<ElementIntegralPostprocessor>();
  params.addRequiredCoupledVar("density_variables", "The density variables corresponding to the phase.");
  params.addCoupledVar("saturation_variables", 1.0, "The saturation variables corresponding to the phase.");
  params.addRequiredCoupledVar("component_mass_fraction_variables", "The mass fraction of the component in this phase");
  return params;
}

ComponentMassPostprocessor::ComponentMassPostprocessor(const std::string & name, InputParameters parameters) :
    ElementIntegralPostprocessor(name, parameters),
    _porosity(getMaterialProperty<Real>("porosity"))
//    _phase_mass(getMaterialProperty<std::vector<Real> >("phase_mass")),
//    _fluid_density(coupledValue("density_variable")),
//    _fluid_saturation(coupledValue("saturation_variable")),
//    _component_mass_fraction(coupledValue("component_mass_fraction_variable"))

{
  unsigned int n = coupledComponents("density_variables");

  _fluid_density.resize(n);
  _fluid_saturation.resize(n);
  _component_mass_fraction.resize(n);

  for (unsigned int i = 0; i < n; ++i)
  {
    _fluid_density[i] = &coupledNodalValue("density_variables", i);
    _fluid_saturation[i] = &coupledNodalValue("saturation_variables", i);
    _component_mass_fraction[i] = &coupledNodalValue("component_mass_fraction_variables", i);
  }
}

Real
ComponentMassPostprocessor::computeQpIntegral()
{
Real mass = 0.;

for (unsigned int i = 0; i < _fluid_density.size(); ++i)
  mass += (*_component_mass_fraction[i])[_qp] * (*_fluid_saturation[i])[_qp] * (*_fluid_density[i])[_qp];

return _porosity[_qp] * mass;
}
