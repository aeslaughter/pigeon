/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Perfectly mobile relative permeability for multiphase flow   */
/* in porous media.                                             */
/*                                                              */
/* Relative permeability is unity for all saturations.          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef RELATIVEPERMEABILITYPERFECTLYMOBILE_H
#define RELATIVEPERMEABILITYPERFECTLYMOBILE_H

#include "RelativePermeability.h"

class RelativePermeabilityPerfectlyMobile;


template<>
InputParameters validParams<RelativePermeabilityPerfectlyMobile>();

class RelativePermeabilityPerfectlyMobile : public RelativePermeability
{
 public:
  RelativePermeabilityPerfectlyMobile(const std::string & name, InputParameters parameters);

  /**
   * Perfectly mobile liquid relative permeability as a function of saturation.
   * @param sat_liq liquid phase saturation
   * @return liquid phase relative permeability (= 1.0)
   */
  Real relativePermLiq(Real sat_liq) const;

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
};

#endif // RELATIVEPERMEABILITYPERFECTLYMOBILE_H
