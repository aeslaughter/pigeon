/****************************************************************/
/* Auxillary kernel to calculate fluid property using the given */
/* Fluid Property UserObject for multiphase flow in porous      */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDPROPERTYAUX_H
#define FLUIDPROPERTYAUX_H

#include "AuxKernel.h"
#include "FluidProperties.h"

class FluidPropertyAux;

template<>
InputParameters validParams<FluidPropertyAux>();

class FluidPropertyAux : public AuxKernel
{
public:

  FluidPropertyAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for fluid property.
   * Note that User Object reference is const.
   */
  const FluidProperties & _fluid_property;

  VariableValue & _pressure;
  VariableValue & _temperature;
  VariableValue & _saturation;
  const MooseEnum & fluid_property_enum;
};

#endif //FLUIDPROPERTYAUX_H
