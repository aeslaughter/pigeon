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
  params.addCoupledVar("component_mass_fraction_variable", 1.0, "The component mass fraction auxiallary variable that is to be summed");
  params.addCoupledVar("phase_saturation_variable", 1.0, "The phase saturation auxiallary variable that is to be summed");
  return params;
}

ComponentMassPostprocessor::ComponentMassPostprocessor(const std::string & name, InputParameters parameters) :
    ElementIntegralPostprocessor(name, parameters),
    _porosity(getMaterialProperty<Real>("porosity")),
    _fluid_density(coupledValue("fluid_density_variable")),
    _component_mass_fraction(coupledValue("component_mass_fraction_variable")),
    _phase_saturation(coupledValue("phase_saturation_variable"))
{}


Real
ComponentMassPostprocessor::computeQpIntegral()
{
  return _porosity[_qp] * _fluid_density[_qp] * _component_mass_fraction[_qp] * _phase_saturation[_qp];
}
