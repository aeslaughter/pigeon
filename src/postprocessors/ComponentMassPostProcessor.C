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
  params.addParam<unsigned int>("phase_index", 0, "The phase index of the fluid that this postprocessor acts on");
  return params;
}

ComponentMassPostprocessor::ComponentMassPostprocessor(const std::string & name, InputParameters parameters) :
    ElementIntegralPostprocessor(name, parameters),
    _porosity(getMaterialProperty<Real>("porosity")),
    _phase_mass(getMaterialProperty<std::vector<Real> >("phase_mass")),
    _phase_index(getParam<unsigned int>("phase_index"))

{}

Real
ComponentMassPostprocessor::computeQpIntegral()
{
  return _porosity[_qp] * _phase_mass[_qp][_phase_index];
}
