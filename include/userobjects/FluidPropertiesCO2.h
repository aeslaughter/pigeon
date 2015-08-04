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
  FluidPropertiesCO2(const InputParameters & parameters);

  /**
   * CO2 molar mass.
   * @return molar mass (kg/mol)
   */
  Real molarMass() const;

  /**
   * CO2 critical pressure.
   * @return critical pressure (Pa)
   */
  Real criticalPressure() const;

  /**
   * CO2 critical temperature.
   * @return critical temperature (C)
   */
  Real criticalTemperature() const;

  /**
   * CO2 density as a function of pressure and temperature.
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return density (kg/m^3)
   */
  virtual Real density(Real pressure, Real temperature, Real xmass = 0) const;

  /**
   * CO2 viscosity as a function of  pressure and temperature.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param density phase density (kg/m^3)
   * @return viscosity (Pa.s)
   */
  virtual Real viscosity(Real pressure, Real temperature, Real density, Real xmass = 0) const;

  /**
   * Henry's law constant coefficients for dissolution of CO2 into water.
   * From Guidelines on the Henry's constant and vapour
   * liquid distribution constant for gases in H20 and D20 at high
   * temperatures, IAPWS (2004).
   *
   * @return constants for Henry's constant (-)
   */
  std::vector<Real> henryConstants() const;

  /**
   * Partial density of dissolved CO2. From Garcia, Density of aqueous
   * solutions of CO2, LBNL-49023 (2001).
   *
   * @param temperature fluid temperature (C)
   * @return partial molar density (kg/m^3)
   */
  Real partialDensity(Real temperature) const;

  /**
   * Density of supercritical CO2. From Ouyang, New correlations for predicting
   * the density and viscosity of supercritical Carbon Dioxide under conditions
   * expected in Carbon Capture and Sequestration operations, The Open Petroleum
   * Engineering Journal, 4, 13-21 (2011)
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return density (kg/m^3)
   */
  Real supercriticalDensity(Real pressure, Real temperature) const;

  /**
   * CO2 gas density as a function of  pressure and temperature.
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return density (kg/m^3)
   */
  Real gasDensity(Real pressure, Real temperature) const;

  /**
   * CO2 gas viscosity as a function of  pressure and temperature.
   * From Fenghour et al., The viscosity of carbon dioxide, J. Phys. Chem. Ref.
   * Data, 27, 31-44 (1998).
   * Note: used up to the critical point. The supercritical viscosity is calculated
   * in supercriticalViscosity(). As a result, the critical enhancement is not
   * implemented.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param density fluid density (kg/m^3)
   * @return viscosity (Pa.s)
   */
  Real gasViscosity(Real pressure, Real temperature, Real density) const;

  /**
   * Viscosity of supercritical CO2. From Ouyang, New correlations for predicting
   * the density and viscosity of supercritical Carbon Dioxide under conditions
   * expected in Carbon Capture and Sequestration operations, The Open Petroleum
   * Engineering Journal, 4, 13-21 (2011)
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return viscosity (Pa.s)
   */
  Real supercriticalViscosity(Real pressure, Real temperature) const;

  /**
   * Derivative of the density of CO2 as a function of
   * pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return derivative of CO2 density (kg/m^3) with respect to pressure (Pa)
   */
  virtual Real dDensity_dP(Real pressure, Real temperature, Real xmass = 0) const;

  /**
   * Derivative of the density of CO2 as a function of
   * temperature.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return derivative of CO2 density (kg/m^3) with respect to temperature (C)
   */
  virtual Real dDensity_dT(Real pressure, Real temperature, Real xmass = 0) const;

  /**
   * Derivative of the density of gaseous CO2 as a function of
   * pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return derivative of CO2 density (kg/m^3) with respect to pressure (Pa)
   */
  Real dGasDensity_dP(Real pressure, Real temperature) const;

  /**
   * Derivative of the density of gaseous CO2 as a function of
   * temperature.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return derivative of CO2 density (kg/m^3) with respect to temperature (C)
   */
  Real dGasDensity_dT(Real pressure, Real temperature) const;

  /**
   * Derivative of the density of supercritical CO2 as a function of
   * pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return derivative of CO2 density (kg/m^3) with respect to pressure (Pa)
   */
  Real dSupercriticalDensity_dP(Real pressure, Real temperature) const;

  /**
   * Derivative of the density of supercritical CO2 as a function of
   * temperature.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return derivative of CO2 density (kg/m^3) with respect to temperature (C)
   */
  Real dSupercriticalDensity_dT(Real pressure, Real temperature) const;

  /**
   * The derivative of CO2 viscosity with respect to density
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param density fluid density (kg/m^3)
   * @return derivative of CO2 viscosity wrt density
   */
  virtual Real dViscosity_dDensity(Real pressure, Real temperature, Real density = 0, Real xmass = 0) const;


 protected:
  /// Conversion of temperature from Celcius to Kelvin
  Real _t_c2k;
  /// Molar mass of CO2
  Real _Mco2;

};

#endif // FLUIDPROPERTIESCO2_H
