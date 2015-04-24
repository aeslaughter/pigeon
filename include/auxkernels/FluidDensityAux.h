/****************************************************************/
/* Auxillary kernel to calculate fluid density using the given  */
/* Fluid Property UserObject for multiphase flow in porous      */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDDENSITYAUX_H
#define FLUIDDENSITYAUX_H

#include "AuxKernel.h"
#include "FluidProperties.h"

class FluidDensityAux;

template<>
InputParameters validParams<FluidDensityAux>();

class FluidDensityAux : public AuxKernel
{
public:

  FluidDensityAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for fluid density.
   * Note that User Object reference is const.
   */
  const FluidProperties & _fluid_property;

  VariableValue & _pressure;
  VariableValue & _saturation;
};

#endif //FLUIDDENSITYAUX_H
