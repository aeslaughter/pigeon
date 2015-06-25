/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef CAPILLARYPRESSUREVANGENUCHTEN_H
#define CAPILLARYPRESSUREVANGENUCHTEN_H

#include "CapillaryPressure.h"

class CapillaryPressureVanGenuchten;

 /**
  * van Genuchten capillary pressure for multiphase flow in porous media.
  *
  * Based on van Genuchten, M. Th., A closed for equation for
  * predicting the hydraulic conductivity of unsaturated soils,
  * Soil Sci. Soc., 44, 892-898 (1980).
  */

template<>
InputParameters validParams<CapillaryPressureVanGenuchten>();

class CapillaryPressureVanGenuchten : public CapillaryPressure
{
 public:
  CapillaryPressureVanGenuchten(const std::string & name, InputParameters parameters);

  /**
   * van Genuchten form of capillary pressure as a function of liquid saturation
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

 protected:

  /// Liquid phase residual saturation
  Real _sat_lr;
  /// Liquid phase fully saturated saturation
  Real _sat_ls;
  /// van Genuchten exponent m
  Real _m;
  /// Maximum capillary pressure
  Real _cp_max;
  /// van Genuchten capillary pressure coefficient
  Real _p0;
};

#endif // CAPILLARYPRESSUREVANGENUCHTEN_H
