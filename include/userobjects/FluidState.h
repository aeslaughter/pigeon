/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATE_H
#define FLUIDSTATE_H

#include "GeneralUserObject.h"
#include "Coupleable.h"
#include "MooseVariable.h"

class FluidState;


template<>
InputParameters validParams<FluidState>();

/**
 * Base class for fluid state calculations of multiphase flow in porous media.
 * The thermophysical properties required are computed at each node of each element
 * of the processor.
 *
 * Dervived classes should implement thermophysicalProperties with the appropriate
 * physics to describe the simulation.
 */
class FluidState : public GeneralUserObject,
public Coupleable

{
 public:
  FluidState(const InputParameters & parameters);

 /**
  * FluidStateProperties class used to store thermophysical properties. This
  * class is used to populate a map of DOF ids and properties, and for accessing
  * the properties given an id.
  */
  class FluidStateProperties
  {
  public:
    std::vector<Real> pressure;
    std::vector<Real> saturation;
    std::vector<Real> density;
    std::vector<Real> viscosity;
    std::vector<Real> relperm;
    std::vector<std::vector<Real> > mass_fraction;
    std::vector<Real> mobility;
    std::vector<Real> ddensity_dp;
    std::vector<Real> ddensity_ds;
    std::vector<std::vector<Real> > ddensity_dx;
    std::vector<Real> drelperm;
    std::vector<Real> dmobility_dp;
    std::vector<Real> dmobility_ds;
    std::vector<std::vector<Real> > dmobility_dx;
  };


  virtual void initialize();
  virtual void execute();
  virtual void finalize();

  /**
   * Calculate all thermophysical properties given the primary variables provided. This is
   * the main routine where specialisations for particular fluids must be included.
   *
   * @param vector of primary variables
   * @param fsp reference to a FluidStateProperties object
   */
  virtual void thermophysicalProperties(std::vector<Real> primary_vars, FluidStateProperties & fsp) = 0;

  /**
   * Retrieves properties at node
   *
   * @param property property name (string)
   * @param nodeid id of node where properties are required
   * @param phase_index index of phase
   * @param component_index index of component (used for mass fraction)
   */
  virtual Real getNodalProperty(std::string property, unsigned int nodeid, unsigned int phase_index, unsigned int component_index = 0) const;

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
    * List of primary variable names
    */
  virtual std::string variableTypes(unsigned int moose_var) const;

  /**
   * List of phase index for each variable
   */
  virtual unsigned int variablePhase(unsigned int moose_var) const = 0;

  /**
   * Primary component index of mass fraction variable
   * @return primary component index
   */
  virtual unsigned int primaryComponentIndex() const;

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
  virtual Real viscosity(Real pressure, Real temperature, Real density, unsigned int phase_index) const = 0;

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
   * Derivative of relative permeabilities of each phase wrt liquid saturation
   *
   * @param saturation liquid saturation
   * @return derivative of relative permeabilities for each phase (-/m)
   */
  virtual std::vector<Real> dRelativePermeability(Real liquid_saturation) const = 0;

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
   * Saturation of all phases given one saturation.
   *
   * @param saturation liquid saturation (-)
   * @return saturation gas saturation (-)
   */
  virtual std::vector<Real> saturation(Real saturation) const = 0;

  /**
   * Sign of the derivative of primary saturation variable with respect to
   * liquid saturation. Used in calculating the pressure gradient of non-primary
   * pressure variables
   *
   * @return sign of derivative
   */

  virtual Real dSaturation_dSl(unsigned int phase_index = 0) const = 0;

  /**
   * Sign of the derivative of a saturation variable with respect to
   * the primary saturation variable. Used in Jacobian calculations
   *
   * @return sign of derivative
   */

  virtual Real dSaturation_dS(unsigned int var) const = 0;

  /**
   * Sign of the derivative of a mass fraction variable with respect to
   * the primary mass fraction variable. Used in Jacobian calculations
   *
   * @return sign of derivative
   */
  virtual Real dMassFraction_dX(unsigned int var) const = 0;

  /**
   * Derivative of fluid  density with respect to fluid pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param phase_index index of phase
   * @return fluid density vector (element for each phase) (kg/m^3)
   */
  virtual Real dDensity_dP(Real pressure, Real temperature, unsigned int phase_index = 0) const = 0;

  /**
   * Derivative of fluid  density with respect to mass fraction.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param phase_index index of phase
   * @return fluid density vector (element for each phase) (kg/m^3)
   */
  virtual Real dDensity_dX(Real pressure, Real temperature, unsigned int phase_index = 0) const = 0;

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

protected:
  MooseMesh & _mesh;

  const unsigned int _qp;

  /// Fluid state properties class to hold thermophysical properties at each node
  std::map<int, FluidStateProperties > _nodal_properties;
  ///  Vector of primary variable values at a node
  std::vector<Real> _primary_vars;

  /// Primary pressure variable
  VariableValue & _pressure;
  /// Primary temperature variable
  VariableValue & _temperature;
  /// Primary mass fracion variable (corresponding to the dissolved component)
  VariableValue & _mass_fraction;

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

  /// MOOSE variable number of primary pressure variable
  unsigned int _pvar;
  /// MOOSE variable number  of primary temperature variable
  unsigned int _tvar;
  /// MOOSE variable number of primary saturation Variable
  unsigned int _svar;
  /// MOOSE variable number of primary mass fraction variable
  unsigned int _xvar;
  /// Component index of primary mass fraction component
  unsigned int _component_index;
  /// Name of primary pressure variable
  std::string _pname;
  /// Name of primary temperature variable
  std::string _tname;
  /// Name of primary saturation Variable
  std::string _sname;
  /// Name of primary mass fraction variable
  std::string _xname;

};

#endif // FLUIDSTATE_H
