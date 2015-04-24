/****************************************************************/
/* Fluid properties base clase for multiphase flow in porous    */
/* media                                                        */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDPROPERTIES_H
#define FLUIDPROPERTIES_H

#include "GeneralUserObject.h"

class FluidProperties;


template<>
InputParameters validParams<FluidProperties>();

/**
 * Base class for fluid properties for multiphase flow in porous media.
 */
class FluidProperties : public GeneralUserObject
{
 public:
  FluidProperties(const std::string & name, InputParameters parameters);

  void initialize();
  void execute();
  void finalize();

  /**
   * Fluid properties that are over-written in the derived class
   * to provide values for the calculations.
   * @param pressure is gas phase pressure.
   * @param temperature is the fluid temperature
   */
  virtual Real fluidDensity(Real pressure, Real temperature) const = 0;
  virtual Real fluidViscosity(Real pressure, Real temperature) const = 0;
//  virtual Real fluidHenryCoefficient(Real temperature) const;


};

#endif // FLUIDPROPERTIES_H
