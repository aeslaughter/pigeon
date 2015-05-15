/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Relative permeability base clase for multiphase flow in      */
/* porous media                                                 */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef RELATIVEPERMEABILITY_H
#define RELATIVEPERMEABILITY_H

#include "GeneralUserObject.h"

class RelativePermeability;


template<>
InputParameters validParams<RelativePermeability>();

/**
 * Base class for relative permeability for multiphase flow in porous media.
 */
class RelativePermeability : public GeneralUserObject
{
 public:
  RelativePermeability(const std::string & name, InputParameters parameters);

  void initialize();
  void execute();
  void finalize();

  /**
   * Liquid relative permeability calculated as a function of saturation.
   * This must be over-ridden in your derived class to provide actual
   * values of relative permeability.
   * @param sat_liq liquid phase saturation
   * @return liquid phase relative permeability
   */
  virtual Real relativePermLiq(Real sat_liq) const = 0;

  /**
   * Gas relative permeability calculated as a function of saturation.
   * This must be over-ridden in your derived class to provide actual
   * values of relative permeability.
   * @param sat_liq liquid phase saturation
   * @return gas phase relative permeability
   */
  virtual Real relativePermGas(Real sat_liq) const = 0;

  /**
   * Derivative of liquid relative permeability with respect to liquid saturation.
   * @param sat_liq liquid phase saturation
   * @return derivative of liquid relative permeability with respect to liquid saturation
   */
  virtual Real dRelativePermLiquid(Real sat_liq) const = 0;

  /**
   * Derivative of gas relative permeability with respect to liquid saturation.
   * @param sat_liq liquid phase saturation
   * @return derivative of gas relative permeability with respect to liquid saturation
   */
  virtual Real dRelativePermGas(Real sat_liq) const = 0;
};

#endif // RELATIVEPERMEABILITY_H
