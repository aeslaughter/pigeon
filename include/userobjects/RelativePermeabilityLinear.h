/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef RELATIVEPERMEABILITYLINEAR_H
#define RELATIVEPERMEABILITYLINEAR_H

#include "RelativePermeability.h"

class RelativePermeabilityLinear;


template<>
InputParameters validParams<RelativePermeabilityLinear>();

class RelativePermeabilityLinear : public RelativePermeability
{
 public:
  RelativePermeabilityLinear(const InputParameters & parameters);

  /**
   * Linear liquid relative permeability as a function of saturation.
   * @param sat_liq liquid phase saturation
   * @return liquid phase relative permeability (= 1.0)
   */
  Real relativePermLiquid(Real sat_liq) const;

  /**
   * Linear gas relative permeability as a function of saturation.
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
};

#endif // RELATIVEPERMEABILITYLINEAR_H
