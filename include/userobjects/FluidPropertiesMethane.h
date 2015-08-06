/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDPROPERTIESMETHANE_H
#define FLUIDPROPERTIESMETHANE_H

#include "FluidPropertiesIdealGas.h"


class FluidPropertiesMethane;


template<>
InputParameters validParams<FluidPropertiesIdealGas>();

class FluidPropertiesMethane : public FluidPropertiesIdealGas
{
 public:
  FluidPropertiesMethane(const InputParameters & parameters);

  /**
   * Methane molar mass.
   * @return molar mass (kg/mol)
   */
  Real molarMass() const;

  /**
   * CH4 gas density as a function of  pressure and temperature assuming an
   * ideal gas.
   *
   * @param pressure gas pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return density (kg/m^3)
   */
  virtual Real density(Real pressure, Real temperature, Real xmass = 0) const;

  /**
   * CH4 gas viscosity as a function of  pressure and temperature.
   * From Irvine Jr, T. F. and Liley, P. E. (1984) Steam and Gas Tables with
   * Computer Equations.
   *
   * @param pressure gas pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return viscosity (Pa.s)
   */
  virtual Real viscosity(Real pressure, Real temperature, Real density = 0, Real xmass = 0) const;

  /**
   * Derivative of the density of gaseous CH4 as a function of
   * pressure.
   *
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @return derivative of CH4 density (kg/m^3) with respect to pressure
   */
  virtual Real dDensity_dP(Real pressure, Real temperature, Real xmass = 0) const;

  /**
   * Derivative of the density of gaseous CH4 as a function of
   * temperature.
   *
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @return derivative of CH4 density (kg/m^3) with respect to temperature
   */
  virtual Real dDensity_dT(Real pressure, Real temperature, Real xmass = 0) const;

  /**
   * Derivative of the viscosity of gaseous CH4 as a function of density
   *
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @param density gas density (kg/m^3)
   * @return derivative of CH4 viscosity wrt density
   */
  virtual Real dViscosity_dDensity(Real pressure, Real temperature, Real density = 0, Real xmass = 0) const;

  /**
   * Derivative of the viscosity of gaseous CH4 as a function of temperature
   *
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @return derivative of CH4 viscosity wrt temperature
   */
  Real dViscosity_dT(Real pressure, Real temperature) const;

  /**
   * Henry's law constant coefficients for dissolution of CH4 into water.
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
  /// Molar mass of pure CH4
  Real _Mch4;

  private:


};

#endif // FLUIDPROPERTIESMETHANE_H
