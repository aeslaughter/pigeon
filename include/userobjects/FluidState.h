/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid state base clase for multiphase flow in porous media   */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
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
   * Liquid density must be over-written in all derived classes.
   *  
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @param xmass vector of component mass fractions (kg/kg)
   * @return liquid density (kg/m^3)
   */
  virtual Real liquidDensity(Real pressure, Real temperature, std::vector<Real> xmass) const = 0;

  /**
   * Gas density must be over-written in all derived classes.
   *  
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @return gas density (kg/m^3)
   */
  virtual Real gasDensity(Real pressure, Real temperature, std::vector<Real> xmass) const = 0;

  /**
   * Liquid viscosity must be over-written in all derived classes.
   *  
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @return liquid viscosity (Pa.s)
   */
  virtual Real liquidViscosity(Real pressure, Real temperature, std::vector<Real> xmass) const = 0;

  /**
   * Gas viscosity must be over-written in all derived classes.
   *  
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @return gas viscosity (Pa.s)
   */
  virtual Real gasViscosity(Real pressure, Real temperature, std::vector<Real> xmass) const = 0;
};

#endif // FLUIDSTATE_H
