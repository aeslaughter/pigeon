/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATE_H
#define FLUIDSTATE_H

#include "GeneralUserObject.h"

class FluidState;


template<>
InputParameters validParams<FluidState>();

/**
 * Base class for fluid state for multiphase flow in porous media.
 */
class FluidState : public GeneralUserObject
{
 public:
  FluidState(const std::string & name, InputParameters parameters);

  void initialize();
  void execute();
  void finalize();

  /**
   * Number of fluid phases
   *
   * @return number of fluid phases
   */
  virtual unsigned int numPhases() const = 0;

  /**
   * Number of components
   *
   * @return number of components
   */
  virtual unsigned int numComponents() const = 0;

  /**
   * Is the simulation isothermal?
   *
   * @return boolean isIsothermal
   */
  virtual bool isIsothermal() const = 0;

  /**
   * Temperature for isothermal simulations
   *
   * @return temperature (C)
   */
  virtual Real temperature() const = 0;

  /**
   * List of primary variable names
   */
  virtual std::vector<std::string> variable_names() const = 0;

  /**
    * List of primary variable names
    */
  virtual std::vector<std::string> variable_types() const = 0;

  /**
   * List of phase index for each variable
   */
  virtual std::vector<unsigned int> variable_phase() const = 0;

  /**
   * Fluid density must be over-written in all derived classes.
   *
   * @param pressure pressure (Pa)
   * @param temperature  temperature (C)
   * @param phase_index index of phase
   * @return fluid density of phase (kg/m^3)
   */
  virtual Real density(Real pressure, Real temperature, unsigned int phase_index) const = 0;

  /**
   * Liquid viscosity must be over-written in all derived classes.
   *
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @param xmass vector of component mass fractions (kg/kg)
   * @return liquid viscosity (Pa.s)
   */
  virtual Real viscosity(Real pressure, Real temperature, unsigned int phase_index) const = 0;

 /**
   * Mass fractions for each component in each phase.
   * 2D vector - one row for each component, one column
   * for each phsae.
   *
   * @return mass fracions (-)
   */
  virtual std::vector<std::vector<Real> > massFractions(Real pressure, Real temperature) const = 0;

  /**
   * Relative permeabilities of each phase
   *
   * @param saturation liquid saturation
   * @return relative permeabilities for each phase (-)
   */
  virtual std::vector<Real> relativePermeability(Real liquid_saturation) const = 0;

  /**
   * Phase pressures
   *
   * @param pressure primary pressure (Pa)
   * @param saturation primary saturation (-)
   * @return pressure phase pressure (Pa)
   */
  virtual std::vector<Real> pressure(Real primary_pressure, Real primary_saturation) const = 0;

  /**
   * Derivative of capillary pressure for each phase with respect to the
   * liquid saturation
   *
   * @param saturation liquid saturation (-)
  * @return derivative of capillary pressure (Pa)
  */
  virtual std::vector<Real> dCapillaryPressure(Real liquid_saturation) const = 0;

  /**
   * Gas saturation given liquid saturation.
   *
   * @param saturation liquid saturation (-)
   * @return saturation gas saturation (-)
   */
  virtual std::vector<Real> saturation(Real liquid_saturation) const = 0;

/**
   * Derivative of fluid  density with respect to fluid pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param xmass vector of component mass fractions (kg/kg)
   * @return fluid density vector (element for each phase) (kg/m^3)
   */
  virtual std::vector<Real> dDensity_dP(Real pressure, Real temperature) const = 0;

/**
 * General formulation for Henry's constant for gas solubility in
 * water. Eq. (3) from Guidelines on the Henry's constant and vapour
 * liquid distribution constant for gases in H20 and D20 at high
 * temperatures, IAPWS (2004).
 *
 * @param temperature water temperature (C)
 * @return Kh Henry's constant (MPa)
 */
virtual Real henry(Real temperature) const = 0;

};

#endif // FLUIDSTATE_H
