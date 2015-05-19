/****************************************************************/
/* Postprocessor to calculate the total mass of each component  */
/* in the model.                                                */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef COMPONENTMASSPOSTPROCESSOR_H
#define COMPONENTMASSPOSTPROCESSOR_H

#include "ElementIntegralPostprocessor.h"

//Forward Declarations
class ComponentMassPostprocessor;

template<>
InputParameters validParams<ComponentMassPostprocessor>();

class ComponentMassPostprocessor : public ElementIntegralPostprocessor
{
public:
  ComponentMassPostprocessor(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();

  MaterialProperty<Real> & _porosity;

  VariableValue & _fluid_density;
  VariableValue & _component_mass_fraction;
  VariableValue & _phase_saturation;

};

#endif // COMPONENTMASSPOSTPROCESSOR_H
