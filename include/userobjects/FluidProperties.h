/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDPROPERTIES_H
#define FLUIDPROPERTIES_H

#include "GeneralUserObject.h"

class FluidProperties;

template<>
InputParameters validParams<FluidProperties>();

/**
 * Base class for fluid properties for multiphase flow in porous media. All
 * FluidProperties dervied classes must override the following thermophysical
 * properties:
 * * density
 * * viscosity
 * * d(density)/d(pressure)
 * * d(density)/d(temperature)
 * * d(viscosty)/d(pressure)
 * * d(viscosity)/d(temperature)
 */

class FluidProperties : public GeneralUserObject
{
 public:
  FluidProperties(const InputParameters & parameters);

  void initialize();
  void execute();
  void finalize();

/**
 * Fluid density must be over-written in all derived classes.
 * Note: not all dervied classes will require the optional mass fraction
 * argument xmass
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @param xmass mass fraction (kg/kg)
 * @return fluid density (kg/m^3)
 */
virtual Real density(Real pressure, Real temperature, Real xmass = 0) const = 0;

/**
 * Fluid viscosity must be over-written in all derived classes.
 * Note: some fluid property classes may use density in the viscosty correlation.
 * Note: not all dervied classes will require the optional mass fraction
 * argument xmass
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @param density fluid density (kg/m^3)
 * @param xmass mass fraction (kg/kg)
 * @return fluid viscosity (Pa.s)
 */
virtual Real viscosity(Real pressure, Real temperature, Real density = 0, Real xmass = 0) const = 0;

/**
 * The derivative of fluid density with respect to pressure must be over-written
 * in all derived classes.
 * Note: not all dervied classes will require the optional mass fraction
 * argument xmass
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @param xmass mass fraction (kg/kg)
 * @return derivative of fluid density with respect to pressure
 */
virtual Real dDensity_dP(Real pressure, Real temperature, Real xmass = 0) const = 0;

/**
 * The derivative of fluid density with respect to temperature must be over-written
 * in all derived classes.
 * Note: not all dervied classes will require the optional mass fraction
 * argument xmass
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @param xmass mass fraction (kg/kg)
 * @return derivative of fluid density with respect to temperature
 */
virtual Real dDensity_dT(Real pressure, Real temperature, Real xmass = 0) const = 0;

/**
 * The derivative of fluid viscosity with respect to pressure must be over-written
 * in all derived classes.
 * Note: some fluid property classes may use density in the viscosty correlation.
 * Note: not all dervied classes will require the optional mass fraction
 * argument xmass
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @param density fluid density (kg/m^3)
 * @param xmass mass fraction (kg/kg)
 * @return derivative of fluid viscosity with respect to pressure
 */
virtual Real dViscosity_dP(Real pressure, Real temperature, Real density = 0, Real xmass = 0) const = 0;
};

#endif // FLUIDPROPERTIES_H
