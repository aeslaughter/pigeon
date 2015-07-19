/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATEMULTIPHASE_H
#define FLUIDSTATEMULTIPHASE_H

#include "FluidProperties.h"
#include "FluidState.h"
#include "RelativePermeability.h"
#include "CapillaryPressure.h"

class FluidStateMultiphase;

/**
 * Default primary variables are gas pressure and liquid saturation.
 * Default components are H2O and CO2.
 *
 * In all vectors, the first element is for liquid, the second for gas
 * In matrices, the first row is for H2O, the second for CO2.
 */


template<>
InputParameters validParams<FluidState>();

class FluidStateMultiphase : public FluidState
{
 public:
  FluidStateMultiphase(const std::string & name, InputParameters parameters);

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
  virtual std::vector<std::string> variableNames() const;


  /**
   * List of primary variable types
   */
  virtual std::vector<std::string> variableTypes() const;

  /**
   * List of phase index for each variable
   */
  virtual std::vector<unsigned int> variablePhase() const;

  /**
   * Thermophysical properties calculated using the primary
   * variables.
   *
   * @param pressure primary pressure (Pa)
   * @param temperature primary temperature (C)
   * @param saturation primary saturation (-)
   * @return thermophysical properties
   */
  virtual std::vector<std::vector<Real> > thermophysicalProperties(Real pressure, Real temperature, Real saturation) const;

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
  virtual Real viscosity(Real pressure, Real temperature, unsigned int phase_index) const;

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
   * Derivative of capillary pressure for each phase with respect to the
   * liquid saturation
   *
   * @param saturation liquid saturation (-)
   * @return derivative of capillary pressure (Pa)
   */
  virtual std::vector<Real> dCapillaryPressure(Real liquid_saturation) const;

  /**
   * Saturation of each phase
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

 protected:

  /**
   * This is the member reference that will hold the User Object
   * value for liquid properties.
   */
  const FluidProperties & _liquid_property;

  /**
   * This is the member reference that will hold the User Object
   * value for gas properties.
   */
  const FluidProperties & _gas_property;

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

#endif // FLUIDSTATEMULTIPHASE_H
