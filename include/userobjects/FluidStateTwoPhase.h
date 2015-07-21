/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATETWOPHASE_H
#define FLUIDSTATETWOPHASE_H

#include "FluidProperties.h"
#include "FluidState.h"
#include "RelativePermeability.h"
#include "CapillaryPressure.h"

class FluidStateTwoPhase;

/**
 * Fluid state for two-phase flow.
 * Default primary variables are gas pressure and liquid saturation.
 * Thermophysical properties are calculated at the nodes. This UserObject
 * must be exectued on linear in the input file.
 *
 * In all 1D vectors, the first element is for liquid, the second for gas
 * In 2D arrays, the rows are for each component, and the colums are for the
 * phases.
 */


template<>
InputParameters validParams<FluidState>();

class FluidStateTwoPhase : public FluidState
{
 public:
  FluidStateTwoPhase(const std::string & name, InputParameters parameters);

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
  virtual Real isothermalTemperature() const;

  /**
   * Temperature variable number for nonisothermal simulations
   *
   * @return MOOSE variable number corresponding to the temperature variable
   */
  virtual unsigned int temperatureVar() const;

  /**
   * Checks if the MOOSE variable number is a primary FluidState variable
   * @param moose_var MOOSE variable number
   * @return bool
   */
  virtual bool isFluidStateVariable(unsigned int moose_var) const;

  /**
   * List of primary variable names
   */
  virtual std::string variableNames(unsigned int moose_var) const;


  /**
   * List of primary variable types
   */
  virtual std::string variableTypes(unsigned int moose_var) const;

  /**
   * List of phase index for each variable
   */
  virtual unsigned int variablePhase(unsigned int moose_var) const;

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
   * Derivative of relative permeabilities of each phase wrt liquid saturation
   *
   * @param saturation liquid saturation
   * @return derivative of relative permeabilities for each phase (-/m)
   */
  virtual std::vector<Real> dRelativePermeability(Real liquid_saturation) const;

  /**
   * Pressure of each phase
   *
   * @param pressure gas pressure (Pa)
   * @param saturation liquid saturation (-)
   * @return pressure liquid phase pressure (Pa)
   */
  virtual std::vector<Real> pressure(Real gas_pressure, Real liquid_saturation, unsigned int phase_index = 0) const;

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
  virtual std::vector<Real> saturation(Real saturation, unsigned int phase_index = 0) const;

  /**
   * Sign of the derivative of primary saturation variable with respect to
   * liquid saturation. Used in calculating the pressure gradient of non-primary
   * pressure variables
   *
   * @return sign of derivative
   */
  virtual Real dSaturation_dSl(unsigned int phase_index = 0) const;

   /**
    * Sign of the derivative of a saturation variable with respect to
    * the primary saturation variable. Used in Jacobian calculations
    *
    * @return sign of derivative
    */
  virtual Real dSaturation_dS(unsigned int var) const;

    /**
     * Derivative of fluid  density with respect to fluid pressure.
     *
     * @param pressure fluid pressure (Pa)
     * @param temperature fluid temperature (C)
     * @param xmass vector of component mass fractions (kg/kg)
     * @return fluid density vector (element for each phase) (kg/m^3)
     */

  virtual Real dDensity_dP(Real pressure, Real temperature, unsigned int phase_index = 0) const;

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
   * Loop over all nodes and calculate the required thermophysical
   * properties.
   */
  virtual void execute();

  /**
   * Return pressure for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase pressure at the node (Pa)
   */
  virtual Real getPressure(unsigned int node_num, unsigned int phase_index) const;
  /**
   * Return saturation for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase saturation at the node (-)
   */
  virtual Real getSaturation(unsigned int node_num, unsigned int phase_index) const;

  /**
   * Return density for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase density at the node (kg/m^3)
   */
  virtual Real getDensity(unsigned int node_num, unsigned int phase_index) const;

  /**
   * Return viscosity for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase viscosity at the node (Pa/s)
   */
  virtual Real getViscosity(unsigned int node_num, unsigned int phase_index) const;

  /**
   * Return relative permeability for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return relative permeability at the node (-)
   */
  virtual Real getRelativePermeability(unsigned int node_num, unsigned int phase_index) const;

  /**
   * Return mass fraction for each component in each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @param component_index index of the component
   * @return component mass fraction at the node (-)
   */
  virtual Real getMassFraction(unsigned int node_num, unsigned int phase_index, unsigned int component_index) const;

  /**
   * Return mobility for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase mobility at the node ()
   */
  virtual Real getMobility(unsigned int node_num, unsigned int phase_index) const;

  /**
   * Return the derivative of relative permeability for each phase at each node
   * wrt liquid saturation
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return derivative of relative permeability at the node (-)
   */
  virtual Real getDRelativePermeability(unsigned int node_num, unsigned int phase_index) const;

  /**
   * Return the derivative of density for each phase at each node
   * wrt pressure
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return derivative of density wrt pressure at the node (-)
   */
  virtual Real getDDensityDP(unsigned int node_num, unsigned int phase_index) const;

  /**
   * Return the derivative of mobility for each phase at each node
   * wrt pressure
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return derivative of mobility wrt pressure at the node (-)
   */
  virtual Real getDMobilityDP(unsigned int node_num, unsigned int phase_index) const;

  /**
   * Return the derivative of mobility for each phase at each node
   * wrt saturation
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return derivative of mobility wrt saturation at the node (-)
   */
  virtual Real getDMobilityDS(unsigned int node_num, unsigned int phase_index) const;


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

  /// Primary pressure variable
  VariableValue & _pressure;
  /// Primary temperature variable
  VariableValue & _temperature;
  /// Primary saturation variable
  VariableValue & _saturation;

  unsigned int _num_components;
  unsigned int _num_phases;
  unsigned int _num_vars;
  bool _not_isothermal;
  std::vector<unsigned int> _varnums;

  unsigned int _p_phase;
  unsigned int _s_phase;
  unsigned int _pvar;
  unsigned int _svar;
  unsigned int _tvar;
  std::string _pname;
  std::string _sname;
  std::string _tname;

  /// Fluid state properties class to hold thermophysical properties at
  /// each node
  std::vector<FluidStateProperties > _fsp;

};

#endif // FLUIDSTATETWOPHASE_H
