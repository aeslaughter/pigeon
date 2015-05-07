/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid state for brine (H20 - NaCl mixture) and CO2.          */
/*                                                              */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDSTATEBRINECO2_H
#define FLUIDSTATEBRINECO2_H

#include "FluidProperties.h"
#include "FluidPropertiesBrine.h"
#include "FluidPropertiesCO2.h"
#include "FluidState.h"

class FluidStateBrineCO2;


template<>
InputParameters validParams<FluidState>();

class FluidStateBrineCO2 : public FluidState
{
 public:
  FluidStateBrineCO2(const std::string & name, InputParameters parameters);

  /**
   * Liquid density using FluidPropertiesBrine and FluidPropertiesCO2 UserObjects.
   *
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @return liquid density (kg/m^3)
   */
  Real liquidDensity(Real pressure, Real temperature, std::vector<Real> xmass) const;

  /**
   * Gas density using FluidPropertiesBrine and FluidPropertiesCO2 UserObjects.
   *
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @return gas density (kg/m^3)
   */
  Real gasDensity(Real pressure, Real temperature, std::vector<Real> xmass) const;
 /**
   * Liquid viscosity must be over-written in all derived classes.
   *  
   * @param pressure liquid pressure (Pa)
   * @param temperature liquid temperature (C)
   * @return liquid viscosity (Pa.s)
   */
  Real liquidViscosity(Real pressure, Real temperature, std::vector<Real> xmass) const;

  /**
   * Gas viscosity must be over-written in all derived classes.
   *  
   * @param pressure gas pressure (Pa)
   * @param temperature gas temperature (C)
   * @return gas viscosity (Pa.s)
   */
  Real gasViscosity(Real pressure, Real temperature, std::vector<Real> xmass) const;


 protected:

/**
   * This is the member reference that will hold the User Object
   * value for brine properties.
   */
  const FluidPropertiesBrine & _brine_property;

/**
   * This is the member reference that will hold the User Object
   * value for CO2 properties.
   */
  const FluidPropertiesCO2 & _co2_property;

};

#endif // FLUIDSTATEBRINECO2_H
