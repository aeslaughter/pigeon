/****************************************************************/
/* van Genuchten relative permeability for multiphase flow in   */
/* porous media.                                                */
/*                                                              */
/* Liquid relative permeability given by                        */
/* van Genuchten, M. Th., A closed for equation for             */
/* predicting the hydraulic conductivity of unsaturated soils,  */
/* Soil Sci. Soc., 44, 892-898 (1980). Gas relative             */
/* given by                                                     */
/* Corey, A. T., The interrelation between gas and oil          */
/* relative permeabilities, Prod. Month. 38-41 (1954).          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef RELATIVEPERMEABILITYVANGENUCHTEN_H
#define RELATIVEPERMEABILITYVANGENUCHTEN_H

#include "RelativePermeability.h"

class RelativePermeabilityVanGenuchten;


template<>
InputParameters validParams<RelativePermeabilityVanGenuchten>();

class RelativePermeabilityVanGenuchten : public RelativePermeability
{
 public:
  RelativePermeabilityVanGenuchten(const std::string & name, InputParameters parameters);

  /**
   * Relative permeability as a function of liquid saturation
   * @param sat_liq is liquid phase saturation
   */
  Real relativePermLiq(Real sat_liq) const;
  Real relativePermGas(Real sat_liq) const;

 protected:

  /// Liquid phase residual saturation
  Real _sat_lr;

  /// Liquid phase fully saturated saturation
  Real _sat_ls;

  /// Gas phase residual saturation
  Real _sat_gr;

  /// van Genuchten exponent m
  Real _m;

};

#endif // RELATIVEPERMEABILITYVANGENUCHTEN_H
