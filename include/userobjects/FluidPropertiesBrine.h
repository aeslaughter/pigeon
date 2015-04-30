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
#include "FluidPropertiesWater.h"

class FluidPropertiesBrine;


template<>
InputParameters validParams<FluidProperties>();

class FluidPropertiesBrine : public FluidProperties
{
 public:
  FluidPropertiesBrine(const std::string & name, InputParameters parameters);

  /**
   * Density of brine.
   * From Driesner, The system H2o-NaCl. Part II: Correlations for molar volume,
   * enthalpy, and isobaric heat capacity from 0 to 1000 C, 1 to 500 bar, and 0 
   * to 1 Xnacl, Geochimica et Cosmochimica Acta 71, 4902-4919 (2007).
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @param xnacl salt mass fraction (-)
   * @return water density (kg/m^3)
   */
  Real density(Real pressure, Real temperature, Real xnacl) const;

  /**
   * Viscosity of brine. 
   *
   * @param temperature water temperature (C)
   * @param density water density (kg/m^3)
   * @param xnacl salt mass fraction (-)
   * @return viscosity (Pa.s)
   */
  Real viscosity(Real temperature, Real density, Real xnacl) const;

 protected:

  /**
   * This is the member reference that will hold the User Object
   * value for fluid property.
   */
  const FluidPropertiesWater & _water_property;

};

#endif // FLUIDPROPERTIESBRINE_H
