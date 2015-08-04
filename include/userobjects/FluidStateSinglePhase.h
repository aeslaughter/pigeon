/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATESINGLEPHASE_H
#define FLUIDSTATESINGLEPHASE_H

#include "FluidState.h"
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
  FluidStateSinglePhase(const InputParameters & parameters);

  /**
   * Number of fluid phases
   *
   * @return number of fluid phases
   */
  virtual unsigned int numPhases() const;

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
   * @param density phase density (kg/m^3)
   * @param phase_index phase index (not used)
   * @return fluid viscosity (Pa.s)
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
   * Sign of the derivative of a mass fraction variable with respect to
   * the primary mass fraction variable. Used in Jacobian calculations
   *
   * @param component_index index of mass fraction component
   * @return sign of derivative
   */
  virtual Real dMassFraction_dX(unsigned int component_index) const;

  /**
   * Derivative of fluid density with respect to fluid pressure.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param phase_index index of phase
   * @return derivative of density wrt pressure
   */

  virtual Real dDensity_dP(Real pressure, Real temperature, unsigned int phase_index = 0) const;

  /**
   * Derivative of fluid density with respect to mass fraction.
   *
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param phase_index index of phase
   * @return derivative of density wrt mass fraction
   */
  virtual Real dDensity_dX(Real pressure, Real temperature, unsigned int phase_index = 0) const;

  /**
   * Derivative of fluid viscosity with respect to density
   *
   * @param pressure phase pressure (Pa)
   * @param temperature fluid temperature (C)
   * @param density phase density (kg/m^3)
   * @param phase_index index of phase
   * @return derivative of viscosity wrt density
   */
  virtual Real dViscosity_dDensity(Real pressure, Real temperature, Real density, unsigned int phase_index = 0) const;

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
   * value for the fluid properties.
   */
  const FluidProperties & _fluid_property;

  // /// Density increase of fluid with dissolved component 1
  Real _density_increase;

};

#endif // FLUIDSTATESINGLEPHASE_H
