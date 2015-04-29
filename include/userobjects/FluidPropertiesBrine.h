/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties of brine (H20 - NaCl mixture).              */
/*                                                              */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDPROPERTIESBRINE_H
#define FLUIDPROPERTIESBRINE_H

#include "FluidProperties.h"

class FluidPropertiesBrine;


template<>
InputParameters validParams<FluidProperties>();

class FluidPropertiesBrine : public FluidProperties
{
 public:
  FluidPropertiesBrine(const std::string & name, InputParameters parameters);

  /**
   * Density of brine.
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @param xnacl salt mass fraction (-)
   * @return water density (kg/m^3)
   */
  Real fluidDensity(Real pressure, Real temperature Real xnacl) const;

  /**
   * Viscosity of brine. 
   *
   * @param temperature water temperature (C)
   * @param density water density (kg/m^3)
   * @param xnacl salt mass fraction (-)
   * @return viscosity (Pa.s)
   */
  Real fluidViscosity(Real temperature, Real density, Real xnacl) const;

 protected:
  /// Temperature of water at the critical point
  Real _t_critical;
  /// Pressure of water at the critical point
  Real _p_critical;
  /// Density of water at the critical point
  Real _rho_critical;
  /// Specific volume of water at the critical point
  Real _v_critical;
  /// Conversion of temperature from Celcius to Kelvin
  Real _t_c2k;
  /// Universal gas constant
  Real _R;
};

#endif // FLUIDPROPERTIESBRINE_H
