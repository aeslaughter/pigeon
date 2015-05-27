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
#include "FluidState.h"
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
InputParameters validParams<FluidState>();

class FluidStateWaterCO2 : public FluidState
{
 public:
  FluidStateWaterCO2(const std::string & name, InputParameters parameters);

  /**
   * Number of fluid phases
   *
   * @return number of fluid phases
   */
  virtual unsigned int numPhases() const;

  /**
   * Number of components
   *
   * @return number of components
   */
  virtual unsigned int numComponents() const;

 /**
   * Is the simulation isothermal?
   *
   * @return boolean isIsothermal
   */
  virtual bool isIsothermal() const;

  /**
   * Temperature for isothermal simulations
   *
   * @return temperature (C)
   */
  virtual Real temperature() const;

  /**
   * List of primary variable names
   */
  virtual std::vector<std::string> variable_names() const;


  /**
   * List of primary variable names
   */
  virtual std::vector<std::string> variable_types() const;

  /**
   * List of phase index for each variable
   */
  virtual std::vector<unsigned int> variable_phase() const;

  /**
   * Fluid density using FluidPropertiesWater and FluidPropertiesCO2 UserObjects.
   *
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @return fluid density (liquid and gas) (kg/m^3)
   */
  virtual std::vector<Real> density(Real pressure, Real temperature) const;

  /**
   * Fluid viscosity using FluidPropertiesWater and FluidPropertiesCO2 UserObjects.
   *
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @return fluid viscosity (liquid and gas) (Pa.s)
   */
  virtual std::vector<Real> viscosity(Real pressure, Real temperature) const;

  /**
   * Mass fractions for each component in each phase.
   * 2D vector - one row for each component, one column
   * for each phsae.
   *
   * @return mass fracions (-)
   */
  virtual std::vector<std::vector<Real> > massFractions(Real pressure, Real temperature) const;

  /**
   * Relative permeabilities of each phase
   *
   * @param saturation liquid saturation
   * @return relative permeabilities for each phase (-)
   */
  virtual std::vector<Real> relativePermeability(Real liquid_saturation) const;

  /**
   * Pressure of each phase
   *
   * @param pressure gas pressure (Pa)
   * @param saturation liquid saturation (-)
   * @return pressure liquid phase pressure (Pa)
   */
  virtual std::vector<Real> pressure(Real gas_pressure, Real liquid_saturation) const;

  /**
   * Saturation of gas phase
   *
   * @param saturation liquid saturation (-)
   * @return saturation gas phase saturation (-)
   */
  virtual std::vector<Real> saturation(Real liquid_saturation) const;

  /**
   * Derivative of fluid  density with respect to fluid pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param xmass vector of component mass fractions (kg/kg)
   * @return fluid density vector (element for each phase) (kg/m^3)
   */
  virtual std::vector<Real> dDensity_dP(Real pressure, Real temperature) const;

 protected:

  /**
   * This is the member reference that will hold the User Object
   * value for brine properties.
   */
  const FluidPropertiesWater & _water_property;

  /**
   * This is the member reference that will hold the User Object
   * value for CO2 properties.
   */
  const FluidPropertiesCO2 & _co2_property;

  /**
   * This is the member reference that will hold the User Object
   * value for relative permeabilities.
   */
  const RelativePermeability & _relative_permeability;

  /**
   * This is the member reference that will hold the User Object
   * value for capillary pressure.
   */
  const CapillaryPressure & _capillary_pressure;

  Real _fluid_temperature;
  bool _is_isothermal;

};

#endif // FLUIDSTATEWATERCO2_H
