/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Constant capillary pressure for multiphase flow in porous    */
/*  media.                                                      */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef CAPILLARYPRESSURECONSTANT_H
#define CAPILLARYPRESSURECONSTANT_H

#include "CapillaryPressure.h"

class CapillaryPressureConstant;


template<>
InputParameters validParams<CapillaryPressureConstant>();

class CapillaryPressureConstant : public CapillaryPressure
{
 public:
  CapillaryPressureConstant(const std::string & name, InputParameters parameters);

  /**
   * Capillary pressure is constant for all liquid saturations
   * @param sat_liq liquid phase saturation
   * @return capillary pressure (Pa)
   */
  Real capillaryPressure(Real sat_liq) const;

 protected:

  /// Constant value for the capillary pressure
  Real _cp;
};

#endif // CAPILLARYPRESSURECONSTANT_H