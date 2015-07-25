/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Corey relative permeability for multiphase flow in porous    */
/*  media.                                                      */
/*                                                              */
/* Based on Corey, A. T., The interrelation between gas and oil */
/* relative permeabilities, Prod. Month. 38-41 (1954).          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef RELATIVEPERMEABILITYCOREY_H
#define RELATIVEPERMEABILITYCOREY_H

#include "RelativePermeability.h"

class RelativePermeabilityCorey;


template<>
InputParameters validParams<RelativePermeabilityCorey>();

class RelativePermeabilityCorey : public RelativePermeability
{
 public:
  RelativePermeabilityCorey(const std::string & name, InputParameters parameters);

  /**
   * Perfectly mobile liquid relative permeability as a function of saturation.
   * @param sat_liq liquid phase saturation
   * @return liquid phase relative permeability (= 1.0)
   */
  Real relativePermLiquid(Real sat_liq) const;

  /**
   * Perfectly mobile gas relative permeability as a function of saturation.
   * @param sat_liq liquid phase saturation
   * @return gas phase relative permeability (= 1.0)
   */
  Real relativePermGas(Real sat_liq) const;

  /**
   * Derivative of liquid relative permeability with respect to liquid saturation.
   * @param sat_liq liquid phase saturation
   * @return derivative of liquid relative permeability with respect to liquid saturation
   */
  Real dRelativePermLiquid(Real sat_liq) const;

  /**
   * Derivative of gas relative permeability with respect to liquid saturation.
   * @param sat_liq liquid phase saturation
   * @return derivative of gas relative permeability with respect to liquid saturation
   */
  Real dRelativePermGas(Real sat_liq) const;

 protected:

  /// Liquid phase residual saturation
  Real _sat_lr;

  /// Gas phase residual saturation
  Real _sat_gr;

};

#endif // RELATIVEPERMEABILITYCOREY_H
