/****************************************************************/
/* Fluid properties of water.                                   */
/*                                                              */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDPROPERTIESWATER_H
#define FLUIDPROPERTIESWATER_H

#include "FluidProperties.h"

class FluidPropertiesWater;


template<>
InputParameters validParams<FluidProperties>();

class FluidPropertiesWater : public FluidProperties
{
 public:
  FluidPropertiesWater(const std::string & name, InputParameters parameters);

  /**
   * Water density and viscosoty as a function of 
   * pressure and temperature.
   * @param pressure is fluid pore pressure (Pa)
   * @param temperature is the fluid temperature (C)
   */
  Real fluidDensity(Real pressure, Real temperature) const;
  Real fluidViscosity(Real pressure, Real temperature) const;
//  Real fluidHenryCoefficient(Real temperature) const;

 protected:

};

#endif // FLUIDPROPERTIESWATER_H
