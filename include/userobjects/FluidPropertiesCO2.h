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
   * Henry's law constant coefficients for dissolution of CO2 into water.
   * From Guidelines on the Henry's constant and vapour
   * liquid distribution constant for gases in H20 and D20 at high
   * temperatures, IAPWS (2004).
   *
   * @return constants for Henry's constant (-)
   */
  std::vector<Real> henryConstants() const;


 protected:
  /// Conversion of temperature from Celcius to Kelvin
  Real _t_c2k;

};

#endif // FLUIDPROPERTIESCO2_H
