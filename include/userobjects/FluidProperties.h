/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties base clase for multiphase flow in porous    */
/* media                                                        */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
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
  virtual Real density(Real pressure, Real temperature) const = 0;

  /**
   * Fluid viscosity must be over-written in all derived classes.
   *  
   * @param pressure fluid pressure (Pa)
   * @param temperature fluid temperature (C)
   * @return fluid viscosity (Pa.s)
   */
  virtual Real viscosity(Real pressure, Real temperature) const = 0;
};

#endif // FLUIDPROPERTIES_H
