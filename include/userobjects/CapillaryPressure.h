/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Capillary pressure base clase for multiphase flow in porous  */
/* media                                                        */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef CAPILLARYPRESSURE_H
#define CAPILLARYPRESSURE_H

#include "GeneralUserObject.h"

class CapillaryPressure;


template<>
InputParameters validParams<CapillaryPressure>();

/**
 * Base class for capillary pressure for multiphase flow in porous media.
 */
class CapillaryPressure : public GeneralUserObject
{
 public:
  CapillaryPressure(const std::string & name, InputParameters parameters);

  void initialize();
  void execute();
  void finalize();

  /**
   * Capillary pressure is calculated as a function of saturation.
   * Ths must be over-ridden in your derived class to provide actual
   * values of capillary pressure.
   * @param sat_liq liquid phase saturation
   * @return capillary pressure (Pa)
   */
  virtual Real capillaryPressure(Real sat_liq) const = 0;

  /**
   * Derivative of capillary pressure with respect to the liquid saturation.
   * @param sat_liq liquid saturation
   * @return derivative of capillary pressure with respect to liquid saturation
   */
  virtual Real dCapillaryPressure(Real sat_liq) const = 0;

  /**
   * Second derivative of capillary pressure with respect to the liquid saturation.
   * @param sat_liq liquid saturation
   * @return second derivative of capillary pressure with respect to liquid saturation
   */
  virtual Real d2CapillaryPressure(Real sat_liq) const = 0;


};

#endif // CAPILLARYPRESSURE_H
