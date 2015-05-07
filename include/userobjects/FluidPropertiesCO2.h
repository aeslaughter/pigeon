/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties of gaseous CO2.                             */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDPROPERTIESCO2_H
#define FLUIDPROPERTIESCO2_H

#include "FluidProperties.h"

class FluidPropertiesCO2;


template<>
InputParameters validParams<FluidProperties>();

class FluidPropertiesCO2 : public FluidProperties
{
 public:
  FluidPropertiesCO2(const std::string & name, InputParameters parameters);

  /**
   * CO2 gas density as a function of  pressure and temperature.
   * @param pressure gas pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return density (kg/m^3)
   */
  Real density(Real pressure, Real temperature) const;

  /**
   * CO2 gas viscosity as a function of  pressure and temperature.
   * @param pressure gas pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return viscosity (Pa.s)
   */
  Real viscosity(Real pressure, Real temperature) const;
//  Real henry(Real temperature) const;

 protected:
  /// Conversion of temperature from Celcius to Kelvin
  Real _t_c2k;

};

#endif // FLUIDPROPERTIESCO2_H
