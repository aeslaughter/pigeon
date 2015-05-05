/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties of supercritical CO2.                       */
/* porous media.                                                */
/*                                                              */
/* Density and viscosity correlation from Liang-Biao Ouyang,    */
/* New correlations for predicting the density and viscosity of */
/* supercritical carbon dioxide under conditions expected in    */
/* carbon capture and sequestration operations. The Open Pet.   */
/* Eng. J., 4, 13-21, 2011.                                     */
/*                                                              */
/* Henry coefficient from Battistelli et al, A fluid property   */
/* module for the TOUGH2 simulator for saline brines with       */
/* non-condensible gas, Proc. 18th Workshop Geothermal Res. Eng.*/
/* 1993.                                                        */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDPROPERTIESCO2_H
#define FLUIDPROPERTIESCO2_H

#include "FluidProperties.h"

class FluidPropertiesCO2;


template<>
InputParameters validParams<FluidProperties>();

class FluidPropertiesCO2 : public FluidProperties
{
 public:
  FluidPropertiesCO2(const std::string & name, InputParameters parameters);

  /**
   * CO2 density, viscosoty and Henry coefficient as a function of
   * pressure and temperature.
   * @param pressure is fluid pore pressure (Pa)
   * @param temperature is the fluid temperature (C)
   */
  Real density(Real pressure, Real temperature) const;
  Real viscosity(Real pressure, Real temperature) const;
//  Real henry(Real temperature) const;

 protected:

};

#endif // FLUIDPROPERTIESCO2_H