/****************************************************************/
/* van Genuchten capillary pressure for multiphase flow in      */
/* porous media.                                                */
/*                                                              */
/* Based on                                                     */
/* van Genuchten, M. Th., A closed for equation for             */
/* predicting the hydraulic conductivity of unsaturated soils,  */
/* Soil Sci. Soc., 44, 892-898 (1980).                          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef CAPILLARYPRESSUREVANGENUCHTEN_H
#define CAPILLARYPRESSUREVANGENUCHTEN_H

#include "CapillaryPressure.h"

class CapillaryPressureVanGenuchten;


template<>
InputParameters validParams<CapillaryPressureVanGenuchten>();

class CapillaryPressureVanGenuchten : public CapillaryPressure
{
 public:
  CapillaryPressureVanGenuchten(const std::string & name, InputParameters parameters);

  /**
   * Capillary pressure as a function of liquid saturation
   * @param sat_liq is liquid phase saturation
   */
  Real capillaryPressure(Real sat_liq) const;

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
