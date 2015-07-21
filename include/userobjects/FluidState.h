/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATE_H
#define FLUIDSTATE_H

#include "NodalUserObject.h"

class FluidState;


template<>
InputParameters validParams<FluidState>();

/**
 * Base class for fluid state for multiphase flow in porous media.
 */
class FluidState : public NodalUserObject
{
 public:
  FluidState(const std::string & name, InputParameters parameters);

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
    std::vector<Real> drelperm;
    std::vector<Real> dmobility_dp;
    std::vector<Real> dmobility_ds;
  };


  virtual void initialize();
  virtual void execute();
  virtual void finalize();
  virtual void threadJoin(const UserObject & uo);

  /**
   * Return pressure for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase pressure at the node (Pa)
   */
  virtual Real getPressure(unsigned int node_num, unsigned int phase_index) const = 0;
  /**
   * Return saturation for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase saturation at the node (-)
   */
  virtual Real getSaturation(unsigned int node_num, unsigned int phase_index) const = 0;

  /**
   * Return density for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase density at the node (kg/m^3)
   */
  virtual Real getDensity(unsigned int node_num, unsigned int phase_index) const = 0;

  /**
   * Return viscosity for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase viscosity at the node (Pa/s)
   */
  virtual Real getViscosity(unsigned int node_num, unsigned int phase_index) const = 0;

  /**
   * Return relative permeability for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return relative permeability at the node (-)
   */
  virtual Real getRelativePermeability(unsigned int node_num, unsigned int phase_index) const = 0;

  /**
   * Return mass fraction for each component in each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @param component_index index of the component
   * @return component mass fraction at the node (-)
   */
  virtual Real getMassFraction(unsigned int node_num, unsigned int phase_index, unsigned int component_index) const = 0;

  /**
   * Return mobility for each phase at each node.
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return phase mobility at the node ()
   */
  virtual Real getMobility(unsigned int node_num, unsigned int phase_index) const = 0;

  /**
   * Return the derivative of relative permeability for each phase at each node
   * wrt liquid saturation
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return derivative of relative permeability at the node (-)
   */
  virtual Real getDRelativePermeability(unsigned int node_num, unsigned int phase_index) const = 0;

  /**
   * Return the derivative of density for each phase at each node
   * wrt pressure
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return derivative of density wrt pressure at the node (-)
   */
  virtual Real getDDensityDP(unsigned int node_num, unsigned int phase_index) const = 0;

  /**
   * Return the derivative of mobility for each phase at each node
   * wrt pressure
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return derivative of mobility wrt pressure at the node (-)
   */
  virtual Real getDMobilityDP(unsigned int node_num, unsigned int phase_index) const = 0;

  /**
   * Return the derivative of mobility for each phase at each node
   * wrt saturation
   *
   * @param node_num node number
   * @param phase_index index of phase
   * @return derivative of mobility wrt saturation at the node (-)
   */
  virtual Real getDMobilityDS(unsigned int node_num, unsigned int phase_index) const = 0;

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
   virtual Real isothermalTemperature() const = 0;

   /**
    * Temperature variable number for nonisothermal simulations
    *
    * @return MOOSE variable number corresponding to the temperature variable
    */
   virtual unsigned int temperatureVar() const = 0;

  /**
   * Checks if the MOOSE variable number is a primary FluidState variable
   * @param moose_var MOOSE variable number
   * @return bool
   */
  virtual bool isFluidStateVariable(unsigned int moose_var) const = 0;


  /**
   * List of primary variable names
   */
  virtual std::string variableNames(unsigned int moose_var) const = 0;

  /**
    * List of primary variable names
    */
  virtual std::string variableTypes(unsigned int moose_var) const = 0;

  /**
   * List of phase index for each variable
   */
  virtual unsigned int variablePhase(unsigned int moose_var) const = 0;

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
  virtual std::vector<Real> pressure(Real primary_pressure, Real primary_saturation, unsigned int phase_index = 0) const = 0;

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
  virtual std::vector<Real> saturation(Real saturation, unsigned int phase_index = 0) const = 0;

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
   * Derivative of fluid  density with respect to fluid pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param xmass vector of component mass fractions (kg/kg)
   * @return fluid density vector (element for each phase) (kg/m^3)
   */
  virtual Real dDensity_dP(Real pressure, Real temperature, unsigned int phase_index = 0) const = 0;

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
