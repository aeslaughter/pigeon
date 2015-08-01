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
  FluidStateTwoPhase(const InputParameters & parameters);

  /**
   * Number of fluid phases
   *
   * @return number of fluid phases
   */
  virtual unsigned int numPhases() const;

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
  virtual Real viscosity(Real pressure, Real temperature, Real density, unsigned int phase_index) const;

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
   * Sign of the derivative of a mass fraction variable with respect to
   * the primary mass fraction variable. Used in Jacobian calculations
   *
   * @return sign of derivative
   */
  virtual Real dMassFraction_dX(unsigned int var) const;

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
   * Derivative of fluid viscosity with respect to density.
   *
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
   * Retrieve the primary saturation variable
   *
   * @return primary saturation variable number
   */
  virtual unsigned int getPrimarySaturationVar() const;

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

    /// Number of phases (can be 1 or 2 in this FluidState UserObject)
  unsigned int _num_phases;

  /// Phase index of primary pressure variable
  unsigned int _p_phase;
  /// Phase index of primary saturation variable
  unsigned int _s_phase;

};

#endif // FLUIDSTATETWOPHASE_H
