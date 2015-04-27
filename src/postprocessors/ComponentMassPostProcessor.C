/****************************************************************/
/* Postprocessor to calculate the total mass of each component  */
/* in the model.                                                */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "ComponentMassPostprocessor.h"

template<>
InputParameters validParams<ComponentMassPostprocessor>()
{
  InputParameters params = validParams<ElementIntegralPostprocessor>();
  params.addRequiredCoupledVar("fluid_density_variable", "The fluid density auxillary variable");
  params.addCoupledVar("component_variable", 1.0, "The variable describing the component that is to be summed");
  return params;
}

ComponentMassPostprocessor::ComponentMassPostprocessor(const std::string & name, InputParameters parameters) :
    ElementIntegralPostprocessor(name, parameters),
    _porosity(getMaterialProperty<Real>("porosity")),
    _fluid_density(coupledValue("fluid_density_variable")),
    _component(coupledValue("component_variable"))
{}


Real
ComponentMassPostprocessor::computeQpIntegral()
{
  return _porosity[_qp] * _fluid_density[_qp] * _component[_qp];
}
