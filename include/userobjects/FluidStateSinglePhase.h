/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATESINGLEPHASE_H
#define FLUIDSTATESINGLEPHASE_H

#include "FluidState.h"
//#include "FluidStateVariables.h"
#include "FluidProperties.h"


class FluidStateSinglePhase;

/**
 * Default primary variable is fluid pressure.
 * Default number of components in 1.
 *
 */

template<>
InputParameters validParams<FluidState>();

class FluidStateSinglePhase : public FluidState
{
 public:
  FluidStateSinglePhase(const std::string & name, InputParameters parameters);

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
    * Primary variable name corresponding to MOOSE variable number
    * @param moose_var MOOSE variable number
    * @return variable name
   */
  virtual std::string variableNames(unsigned int moose_var) const;


  /**
    * Primary variable type corresponding to MOOSE variable number
    * @param moose_var MOOSE variable number
    * @return variable type
   */
  virtual std::string variableTypes(unsigned int moose_var) const;

  /**
   * Primary variable phase index of MOOSE variable number
   * @param moose_var MOOSE variable number
   * @return variable phase
   */
  virtual unsigned int variablePhase(unsigned int moose_var) const;

  /**
   * Fluid density
   *
   * @param pressure phase pressure (Pa)
   * @param temperature temperature (C)
   * @param phase_index phase index (not used)
   * @return fluid density (kg/m^3)
   */
  virtual Real density(Real pressure, Real temperature, unsigned int phase_index) const;

  /**
   * Fluid viscosity
   *
   * @param pressure phase pressure (Pa)
   * @param temperature temperature (C)
   * @param phase_index phase index (not used)
   * @return fluid viscosity (Pa.s)
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
   * Pressure of liquid phase
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
  * @return derivative of capillary pressure (Pa/m)
  */
  virtual std::vector<Real> dCapillaryPressure(Real liquid_saturation) const;

  /**
   * Saturation of gas phase
   *
   * @param saturation liquid saturation (-)
   * @return saturation gas phase saturation (-)
   */
  virtual std::vector<Real> saturation(Real saturation) const;

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
   * General formulation for Henry's constant for gas solubility in
   * water. Eq. (3) from Guidelines on the Henry's constant and vapour
   * liquid distribution constant for gases in H20 and D20 at high
   * temperatures, IAPWS (2004).
   *
   * @param temperature water temperature (C)
   * @return Kh Henry's constant (MPa)
   */
  virtual Real henry(Real temperature) const {return 0.;};


  /**
   * Initialize the property map each time this UserObject is called
   */
  virtual void initialize();

  /**
   * Loop over all nodes on each processor and calculates the thermophysical
   * properties at each node.
   */
  virtual void execute();

  /**
   * Calculate all thermophysical properties given the primary variables provided. This is
   * the main routine where specialisations for particular fluids must be included.
   *
   * @param vector of primary variables
   * @param fsp reference to a FluidStateProperties object
   */
  virtual void thermophysicalProperties(std::vector<Real> primary_vars, FluidStateProperties & fsp);

  /**
   * Retrieves properties at node
   *
   * @param property property name (string)
   * @param nodeid id of node where properties are required
   * @param phase_index index of phase
   * @param component_index index of component (used for mass fraction)
   */
  virtual Real getNodalProperty(std::string property, unsigned int nodeid, unsigned int phase_index, unsigned int component_index = 0) const;

 protected:

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid properties.
   */
  const FluidProperties & _fluid_property;


  /// Primary pressure variable
  VariableValue & _pressure;
  /// Primary temperature variable
  VariableValue & _temperature;

  /// Number of components
  unsigned int _num_components;
  /// Number of phases (1 in this FluidState UserObject)
  unsigned int _num_phases;
  /// Number of primary variables
  unsigned int _num_vars;
  /// Bool to flag if the simulation is isothermal
  bool _not_isothermal;
  /// Vector of MOOSE primary variable numbers
  std::vector<unsigned int> _varnums;

  /// Phase index of primary pressure variable
  unsigned int _pvar;
  /// Phase index of primary temperature variable
  unsigned int _tvar;
  /// Name of primary pressure variable
  std::string _pname;
  /// Name of primary temperature variable
  std::string _tname;

  /// Fluid state properties class to hold thermophysical properties at each node
  std::map<int, FluidStateProperties > _nodal_properties;
  ///  Vector of primary variable values at a node
  std::vector<Real> _primary_vars;
};

#endif // FLUIDSTATESINGLEPHASE_H