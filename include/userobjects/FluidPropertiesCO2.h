/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
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

  /**
   * Derivative of the density of gaseous CO2 as a function of
   * pressure.
   *
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @return derivative of CO2 density (kg/m^3) with respect to pressure
   */
  Real dDensity_dP(Real pressure, Real temperature) const;

  /**
   * Henry's law coefficient for dissolution of CO2 into water.
   * From Battistelli et al, A fluid property module for the TOUGH2 simulator
   * for saline brines with non-condensible gas. Proc. Eighteenth Workshop on
   * Geothermal Reservoir Engineering, Stanford University, 1993.
   *
   * @param temperature fluid temperature (C)
   * @return Henry's coefficient Kh (Pa)
   */
  Real henry(Real temperature) const;


 protected:
  /// Conversion of temperature from Celcius to Kelvin
  Real _t_c2k;

};

#endif // FLUIDPROPERTIESCO2_H
