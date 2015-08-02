/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDPROPERTIESIDEALGAS_H
#define FLUIDPROPERTIESIDEALGAS_H

#include "FluidProperties.h"

class FluidPropertiesIdealGas;


template<>
InputParameters validParams<FluidProperties>();

class FluidPropertiesIdealGas : public FluidProperties
{
 public:
  FluidPropertiesIdealGas(const InputParameters & parameters);

  /**
   * Ideal gas density as a function of  pressure, temperature and molar mass.
   *
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @param molar mass gas molar mass (kg/mol)
   * @return density (kg/m^3)
   */
  virtual Real density(Real pressure, Real temperature, Real molar_mass) const;

  /**
   * Derivative of the density of an ideal gas as a function of
   * pressure.
   *
   * @param temperature gas temperature (C)
   * @param molar mass gas molar mass (kg/mol)
   * @return derivative of CO2 density (kg/m^3) with respect to pressure
   */
  virtual Real dDensity_dP(Real pressure, Real temperature, Real molar_mass) const;

  /**
   * Derivative of the density of an ideal gas as a function of
   * temperature.
   *
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @param molar mass gas molar mass (kg/mol)
   * @return derivative of CO2 density (kg/m^3) with respect to pressure
   */
  virtual Real dDensity_dT(Real pressure, Real temperature, Real molar_mass) const;

  virtual Real viscosity(Real pressure, Real temperature, Real molar_mass, Real xmass = 0) const;

  virtual Real dViscosity_dP(Real pressure, Real temperature, Real molar_mass, Real xmass = 0) const;


 protected:
 /// Conversion of temperature from Celcius to Kelvin
 Real _t_c2k;
 /// Gas constant
 Real _R;

};

#endif // FLUIDPROPERTIESIDEALGAS_H
