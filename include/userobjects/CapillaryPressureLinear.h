/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef CAPILLARYPRESSURELINEAR_H
#define CAPILLARYPRESSURELINEAR_H

#include "CapillaryPressure.h"

class CapillaryPressureLinear;


template<>
InputParameters validParams<CapillaryPressureLinear>();

class CapillaryPressureLinear : public CapillaryPressure
{
 public:
  CapillaryPressureLinear(const InputParameters & parameters);

  /**
   * Capillary pressure
   * @param sat_liq liquid phase saturation
   * @return capillary pressure (Pa)
   */
  Real capillaryPressure(Real sat_liq) const;

  /**
   * Derivative of capillary pressure with respect to the liquid saturation.
   * @param sat_liq liquid saturation
   * @return derivativer of capillary pressure with respect to liquid saturation
   */
  Real dCapillaryPressure(Real sat_liq) const;

  /**
   * Second derivative of capillary pressure with respect to the liquid saturation.
   * @param sat_liq liquid saturation
   * @return second derivative of capillary pressure with respect to liquid saturation
   */
  Real d2CapillaryPressure(Real sat_liq) const;


 protected:

 /// Maximum capillary pressure
 Real _cp_max;
};

#endif // CAPILLARYPRESSURELINEAR_H
