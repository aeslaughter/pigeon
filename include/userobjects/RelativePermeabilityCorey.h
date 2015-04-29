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
   * Relative permeability as a function of liquid saturation
   * @param sat_liq is liquid phase saturation
   */
  Real relativePermLiq(Real sat_liq) const;
  Real relativePermGas(Real sat_liq) const;

 protected:

  /// Liquid phase residual saturation
  Real _sat_lr;

  /// Gas phase residual saturation
  Real _sat_gr;

};

#endif // RELATIVEPERMEABILITYCOREY_H
