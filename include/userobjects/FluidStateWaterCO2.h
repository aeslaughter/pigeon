/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATEWATERCO2_H
#define FLUIDSTATEWATERCO2_H

#include "FluidProperties.h"
#include "FluidPropertiesWater.h"
#include "FluidPropertiesCO2.h"
#include "FluidStateTwoPhase.h"
#include "RelativePermeability.h"
#include "CapillaryPressure.h"

class FluidStateWaterCO2;

/**
 * Default primary variables are gas pressure and liquid saturation.
 * Default components are H2O and CO2.
 *
 * In all vectors, the first element is for liquid, the second for gas
 * In matrices, the first row is for H2O, the second for CO2.
 */

template<>
InputParameters validParams<FluidStateTwoPhase>();

class FluidStateWaterCO2 : public FluidStateTwoPhase
{
 public:
  FluidStateWaterCO2(const InputParameters & parameters);

  /**
   * Fluid density for each phase
   *
   * @param pressure phase pressure (Pa)
   * @param temperature temperature (C)
   * @return fluid density (kg/m^3)
   */
  virtual Real density(Real pressure, Real temperature, unsigned int phase_index) const;

  /**
   * Fluid viscosity for each phase
   *
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @return fluid viscosity (liquid and gas) (Pa.s)
   */
  virtual Real viscosity(Real pressure, Real temperature, Real density, unsigned int phase_index) const;

  /**
   * Derivative of fluid  density with respect to fluid pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param phase_index index of phase
   * @return fluid density vector (element for each phase) (kg/m^3)
   */
  virtual Real dDensity_dP(Real pressure, Real temperature, unsigned int phase_index = 0) const;

  /**
   * Derivative of fluid  density with respect to mass fraction.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param phase_index index of phase
   * @return fluid density vector (element for each phase) (kg/m^3)
   */
  virtual Real dDensity_dX(Real pressure, Real temperature, unsigned int phase_index = 0) const;

  /**
   * Derivative of fluid viscosity with respect to density
   *
   * @param pressure fluid pressure (Pa)
   * @param density fluid density (kg/m^3)
   * @param temperature fluid temperature (C)
   * @param phase_index index of phase
   * @return derivative of viscosity wrt density
   */
  virtual Real dViscosity_dDensity(Real pressure, Real temperature, Real density, unsigned int phase_index = 0) const;

  /**
   * Henry's law set to zero for immiscible fluids
   *
   * @param temperature water temperature (C)
   * @param fitting constants for each gas
   * @return Kh Henry's constant (MPa)
   */
  virtual Real henry(Real temperature) const;

  /**
   * Mass fraction of dissolved gas in the liquid phase.
   * Set to zero for immiscible fluids
   *
   * @param pressure CO2 partial pressure (Pa)
   * @param temperature temperature (C)
   * @return dissolved mass fraction (-)
   */
  virtual Real dissolved(Real pressure, Real temperature) const;

  /**
   * Calculate all thermophysical properties given the primary variables provided. This is
   * the main routine where specialisations for particular fluids must be included.
   *
   * @param vector of primary variables
   * @param fsp reference to a FluidStateProperties object
   */
  virtual void thermophysicalProperties(std::vector<Real> primary_vars, FluidStateProperties & fsp);

 protected:

  /**
   * This is the member reference that will hold the User Object
   * value for water properties.
   */
  const FluidPropertiesWater & _water_property;

  /**
   * This is the member reference that will hold the User Object
   * value for CO2 properties.
   */
  const FluidPropertiesCO2 & _co2_property;

  /// Number of phases (can be 1 or 2 in this FluidState UserObject)
  unsigned int _num_phases;
  /// Molar mass of water
  Real _Mh2o;
  /// Molar mass of CO2
  Real _Mco2;


};

#endif // FLUIDSTATEWATERCO2_H
