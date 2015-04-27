/****************************************************************/
/* Postprocessor to calculate the mass flux over a side.        */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef SIDEMASSFLUXINTEGRALPOSTPROCESSOR_H
#define SIDEMASSFLUXINTEGRALPOSTPROCESSOR_H

#include "SideIntegralVariablePostprocessor.h"

//Forward Declarations
class SideMassFluxIntegralPostprocessor;

template<>
InputParameters validParams<SideMassFluxIntegralPostprocessor>();

class SideMassFluxIntegralPostprocessor : public SideIntegralVariablePostprocessor
{
public:
  SideMassFluxIntegralPostprocessor(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();
  
  MaterialProperty<RealTensorValue> & _permeability;
  MaterialProperty<RealVectorValue> & _gravity;

  VariableValue & _fluid_density;
  VariableValue & _fluid_viscosity;
  VariableValue & _component;
  
};

#endif // SIDEMASSFLUXINTEGRALPOSTPROCESSOR_H
