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
  CapillaryPressureVanGenuchten(const InputParameters & parameters);

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

  /**
   * Second derivative of capillary pressure with respect to the liquid saturation.
   * @param sat_liq liquid saturation
   * @return second derivative of capillary pressure with respect to liquid saturation
   */
  Real d2CapillaryPressure(Real sat_liq) const;

  /**
   * Unmodified version of van Genuchten capillary pressure as a function of
   * liquid saturation
   * @param sat_liq liquid phase saturation
   * @return capillary pressure (Pa)
   */
  Real capillaryPressureUM(Real sat_liq) const;

  /**
   * Unmodified version of the derivative of the van Genuchten capillary pressure
   * with respect to liquid saturation
   * @param sat_liq liquid saturation
   * @return derivativer of capillary pressure with respect to liquid saturation
   */
  Real dCapillaryPressureUM(Real sat_liq) const;

  /**
  * Unmodified version of the second derivative of the van Genuchten capillary pressure
  * with respect to liquid saturation
  * @param sat_liq liquid saturation
   * @return derivativer of capillary pressure with respect to liquid saturation
   */
  Real d2CapillaryPressureUM(Real sat_liq) const;

  /**
   * Cubic spline interpolation at low liquid saturation
   */
  Real cSpline0(Real sat_liq, Real sat0, Real sat1) const;

  /**
   * Derivative of cubic spline interpolation at low liquid saturation
   */
  Real dCSpline0(Real sat_liq, Real sat0, Real sat1) const;

  /**
   * Second derivative of cubic spline interpolation at low liquid saturation
   */
  Real d2CSpline0(Real sat_liq, Real sat0, Real sat1) const;

  /**
   * Cubic spline interpolation at high liquid saturation
   */
  Real cSpline1(Real sat_liq, Real sat0, Real sat1) const;

  /**
   * Derivative of cubic spline interpolation at high liquid saturation
   */
  Real dCSpline1(Real sat_liq, Real sat0, Real sat1) const;

  /**
   * Second derivative of cubic spline interpolation at high liquid saturation
   */
  Real d2CSpline1(Real sat_liq, Real sat0, Real sat1) const;


  /**
   * Location of intercept between capillary pressure cutoff (_cp_max)
   */
  Real cutoffIntercept() const;

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
  /// Interpolation window for spline interpolation near residual saturation
  Real _delta_s;
};

#endif // CAPILLARYPRESSUREVANGENUCHTEN_H
