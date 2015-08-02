/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDPROPERTIESWATER_H
#define FLUIDPROPERTIESWATER_H

#include "FluidProperties.h"

class FluidPropertiesWater;


template<>
InputParameters validParams<FluidProperties>();

class FluidPropertiesWater : public FluidProperties
{
 public:
  FluidPropertiesWater(const InputParameters & parameters);

  /**
   * Water molar mass.
   * @return molar mass (kg/mol)
   */
  Real molarMass() const;

  /**
   * Density of water
   * From IAPWS IF97 Revised Release on the IAPWS Industrial
   * Formulation 1997 for the Thermodynamic Properties of Water
   * and Steam.
   *
   * Valid for 273.15 K <= T <= 1073.15 K, p <= 100 MPa
   *          1073.15 K <= T <= 2273.15 K, p <= 50 Mpa
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @return water density (kg/m^3)
   */
  Real density(Real pressure, Real temperature, Real xmass = 0) const;

  /**
   * Viscosity of water.
   * Eq. (10) from Release on the IAPWS Formulation 2008 for the
   * Viscosity of Ordinary Water Substance.
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @param density water density (kg/m^3)
   * @return viscosity (Pa.s)
   */
  Real viscosity(Real pressure, Real temperature, Real density, Real xmass = 0) const;

  /** Saturation pressure as a function of temperature.
   *
   * Eq. (30) from Revised Release on the IAPWS Industrial
   * Formulation 1997 for the Thermodynamic Properties of Water
   * and Steam, IAPWS 2007.
   *
   * Valid for 273.15 K <= t <= 647.096 K
   *
   * @param temperature water temperature (C)
   * @return saturation pressure (Pa)
   */
  Real pSat(Real temperature) const;

  /** Saturation temperature as a function of pressure.
   *
   * Eq. (31) from Revised Release on the IAPWS Industrial
   * Formulation 1997 for the Thermodynamic Properties of Water
   * and Steam, IAPWS 2007.
   *
   * Valid for 611.213 Pa <= p <= 22.064 MPa
   *
   * @param pressure water pressure (Pa)
   * @return saturation temperature (C)
   */
  Real tSat(Real pressure) const;

  /** Auxillary equation for the boundary between regions 2 and 3.
   *
   * Eq. (5) from Revised Release on the IAPWS Industrial
   * Formulation 1997 for the Thermodynamic Properties of Water
   * and Steam, IAPWS 2007.
   *
   * @param temperature water temperature (C)
   * @return pressure (Pa) on the boundary between region 2 and 3
   */
  Real b23p(Real temperature) const;

  /** Auxillary equation for the boundary between regions 2 and 3.
   *
   * Eq. (6) from Revised Release on the IAPWS Industrial
   * Formulation 1997 for the Thermodynamic Properties of Water
   * and Steam, IAPWS 2007.
   *
   * @param pressure water pressure (Pa)
   * @return temperature (C) on the boundary between regions 2 and 3
   */
  Real b23t(Real pressure) const;

  /** Density function for Region 1 - single phase liquid region
   *
   * From Eq. (7) From Revised Release on the IAPWS Industrial
   * Formulation 1997 for the Thermodynamic Properties of Water
   * and Steam, IAPWS 2007.
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @return density (kg/m^3) in region 1
   */
  Real densityRegion1(Real pressure, Real temperature) const;

  /** Density function for Region 2 - superheated steam
   *
   * From Eq. (15) From Revised Release on the IAPWS Industrial
   * Formulation 1997 for the Thermodynamic Properties of Water
   * and Steam, IAPWS 2007.
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @return density (kg/m^3) in region 2
   */
  Real densityRegion2(Real pressure, Real temperature) const;

  /** Density function for Region 3 - supercritical water and steam
   *
   * To avoid iteration, use the backwards equations for region 3
   * from Revised Supplementary Release on Backward Equations for
   * Specific Volume as a Function of Pressure and Temperature v(p,T)
   * for Region 3 of the IAPWS Industrial Formulation 1997 for the
   * Thermodynamic Properties of Water and Steam.
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @return density (kg/m^3) in region 3
   */
  Real densityRegion3(Real pressure, Real temperature) const;

  /**
   * Derivative of the density of water with respect to pressure.
   * From IAPWS IF97 Revised Release on the IAPWS Industrial
   * Formulation 1997 for the Thermodynamic Properties of Water
   * and Steam.
   *
   * Valid for 273.15 K <= T <= 1073.15 K, p <= 100 MPa
   *          1073.15 K <= T <= 2273.15 K, p <= 50 Mpa
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @return derivative of water density (kg/m^3) with respect to pressure
   */
  Real dDensity_dP(Real pressure, Real temperature, Real xmass = 0) const;

  /**
   * Derivative of density function for Region 1 - single phase liquid region
   * with respect to presure.
   * From Revised Release on the IAPWS Industrial Formulation 1997 for the
   * Thermodynamic Properties of Water and Steam, IAPWS 2007.
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @return derivative of density (kg/m^3) in region 1 with respect to pressure
   */
  Real dDensityRegion1_dP(Real pressure, Real temperature) const;

  /**
   * Derivative of density function for Region 2 - superheated steam
   * with respect to presure.
   * From Revised Release on the IAPWS Industrial Formulation 1997 for the
   * Thermodynamic Properties of Water and Steam, IAPWS 2007.
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @return derivative of density (kg/m^3) in region 2 with respect to pressure
   */
  Real dDensityRegion2_dP(Real pressure, Real temperature) const;

  /**
   * Derivative of viscosity with respect to pressure. Derived from
   * Eq. (10) from Release on the IAPWS Formulation 2008 for the
   * Viscosity of Ordinary Water Substance.
   *
   * @param pressure water pressure (Pa)
   * @param temperature water temperature (C)
   * @param density water density (kg/m^3)
   * @return dViscosity_dpressure
   */
  Real dViscosity_dP(Real pressure, Real temperature, Real density, Real xmass = 0) const;

  Real dDensity_dT(Real pressure, Real temperature, Real xmass = 0) const;



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
  /// Molar mass of water
  Real _Mh2o;
  /// Universal gas constant
  Real _R;
  // Constants
  std::vector<Real> _n1;
  std::vector<int> _I1;
  std::vector<int> _J1;
  std::vector<Real> _n02;
  std::vector<Real> _n2;
  std::vector<int> _J02;
  std::vector<int> _I2;
  std::vector<int> _J2;
  std::vector<int> _iv;
  std::vector<int> _jv;
  std::vector<Real> _h0v;
  std::vector<Real> _h1v;

};

#endif // FLUIDPROPERTIESWATER_H
