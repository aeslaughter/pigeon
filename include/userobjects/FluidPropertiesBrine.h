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
   * @param pressure brine pressure (Pa)
   * @param temperature brine temperature (C)
   * @param xnacl salt mass fraction (-)
   * @return water density (kg/m^3)
   */
  Real brineDensity(Real pressure, Real temperature, Real xnacl) const;

  /**
   * Viscosity of brine. 
   * From Phillips et al, A technical databook for geothermal energy utilization,
   * LbL-12810 (1981).
   *
   * @param temperature brine temperature (C)
   * @param density water density (kg/m^3)
   * @param xnacl salt mass fraction (-)
   * @return viscosity (Pa.s)
   */
  Real brineViscosity(Real temperature, Real density, Real xnacl) const;

  /**
   * Density of halite (solid NaCl)
   * From Driesner, The system H2o-NaCl. Part II: Correlations for molar volume,
   * enthalpy, and isobaric heat capacity from 0 to 1000 C, 1 to 500 bar, and 0 
   * to 1 Xnacl, Geochimica et Cosmochimica Acta 71, 4902-4919 (2007).
   *
   * @param pressure pressure (Pa)
   * @param temperature halite temperature (C)
   * @return density (kg/m^3)
   */
  Real haliteDensity(Real pressure, Real temperature) const;

  /**
   * Halite solubility 
   * Originally from Potter et al., A new method for determining the solubility
   * of salts in aqueous solutions at elevated temperatures, J. Res. U.S. Geol.
   * Surv., 5, 389-395 (1977). Equation describing halite solubility is repeated
   * in Chou, Phase relations in the system NaCI-KCI-H20. III: Solubilities of 
   * halite in vapor-saturated liquids above 445°C and redetermination of phase 
   * equilibrium properties in the system NaCI-HzO to 1000°C and 1500 bars,
   * Geochimica et Cosmochimica Acta 51, 1965-1975 (1987).
   *
   * @param temperature temperature (C)
   * @return halite solubility (kg/kg)
   *
   * This correlation is valid for 0 <= T << 424.5 C
   */
  Real haliteSolubility(Real temperature) const;

  /**
   * Brine vapour pressure
   * From Haas, Physical properties of the coexisting phases and thermochemical
   * properties of the H20 component in boiling NaCl solutions, Geological Survey
   * Bulletin, 1421-A (1976).
   *
   * @param temperature brine temperature (C)
   * @param xnacl salt mass fraction (-)
   * @return brine vapour pressure (Pa)
   */
  Real brinePSat(Real temperature, Real xnacl) const;
 

 protected:

  /**
   * This is the member reference that will hold the User Object
   * value for fluid property.
   */
  const FluidPropertiesWater & _water_property;

  /// Molar mass of pure water
  Real _Mh2o;
  /// Molar mass of NaCl
  Real _Mnacl;
  /// Conversion of temperature from Celcius to Kelvin
  Real _t_c2k;

};

#endif // FLUIDPROPERTIESBRINE_H
