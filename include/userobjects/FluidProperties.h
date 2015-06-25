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
 * Base class for fluid properties for multiphase flow in porous media.
 */

class FluidProperties : public GeneralUserObject
{
 public:
  FluidProperties(const std::string & name, InputParameters parameters);

  void initialize();
  void execute();
  void finalize();

/**
 * Fluid density must be over-written in all derived classes.
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @return fluid density (kg/m^3)
 */
virtual Real density(Real pressure, Real temperature) const;
virtual Real density(Real pressure, Real temperature, Real xmass) const;

/**
 * Fluid viscosity must be over-written in all derived classes.
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @return fluid viscosity (Pa.s)
 */
virtual Real viscosity(Real pressure, Real temperature) const;
virtual Real viscosity(Real pressure, Real temperature, Real xmass) const;

/**
 * The derivative of fluid density with respect to pressure must be over-written
 *  in all derived classes.
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @return derivative of fluid density with respect to pressure
 */
virtual Real dDensity_dP(Real pressure, Real temperature) const;

/**
 * The derivative of fluid density with respect to temperature must be over-written
 *  in all derived classes.
 *
 * @param pressure fluid pressure (Pa)
 * @param temperature fluid temperature (C)
 * @return derivative of fluid density with respect to temperature
 */
virtual Real dDensity_dT(Real pressure, Real temperature) const;
};

#endif // FLUIDPROPERTIES_H
