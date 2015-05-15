/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid state for brine (H20 - NaCl mixture) and CO2.          */
/*                                                              */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDSTATEBRINECO2_H
#define FLUIDSTATEBRINECO2_H

#include "FluidProperties.h"
#include "FluidPropertiesBrine.h"
#include "FluidPropertiesCO2.h"
#include "FluidState.h"
#include "RelativePermeability.h"
#include "CapillaryPressure.h"

class FluidStateBrineCO2;


template<>
InputParameters validParams<FluidState>();

class FluidStateBrineCO2 : public FluidState
{
 public:
  FluidStateBrineCO2(const std::string & name, InputParameters parameters);

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
   *  Temperature for isothermal simulations
   *
   * @return temperature (C)
   */
  virtual Real temperature() const;

  /**
   * Fluid density using FluidPropertiesBrine and FluidPropertiesCO2 UserObjects.
   *
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @return fluid density (gas and liquid) (kg/m^3)
   */
  virtual std::vector<Real> density(Real pressure, Real temperature) const;

  /**
   * Fluid viscosity must be over-written in all derived classes.
   *  
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @return liquid viscosity (Pa.s)
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
   * Pressure of liquid phase
   *
   * @param pressure gas pressure (Pa)
   * @param saturation liquid saturation (-)
   * @return pressure liquid phase pressure (Pa)
   */
  virtual Real pressure(Real gas_pressure, Real liquid_saturation) const;

  /**
   * Saturation of gas phase
   *
   * @param saturation liquid saturation (-)
   * @return saturation gas phase saturation (-)
   */
  virtual Real saturation(Real liquid_saturation) const;

 protected:

  /**
   * This is the member reference that will hold the User Object
   * value for brine properties.
   */
  const FluidPropertiesBrine & _brine_property;

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

#endif // FLUIDSTATEBRINECO2_H
