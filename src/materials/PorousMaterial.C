/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Porous material for multiphase flow                          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "PorousMaterial.h"

template<>
InputParameters validParams<PorousMaterial>()
{
  InputParameters params = validParams<Material>();

  params.addRequiredParam<Real>("density", "Density of the material (kg/m^3)");
  params.addRequiredParam<Real>("porosity", "Porosity of the material (must be between 0 and 1)");
  params.addRequiredParam<RealTensorValue>("permeability", "The permeability tensor (m^2)");
  params.addRequiredParam<RealVectorValue>("gravity", "Gravitational acceleration vector downwards (m/s^2)");
  params.addRequiredParam<Real>("diffusivity", "Diffusivity (m^2/s");

  /// MooseEnum for relative permeability choice. Default choice 'unity' (perfectly mobile, krg=krl=1).
//  MooseEnum relperm_model ("unity, linear, Corey, vanGenuchten", "unity");
//  params.addParam<MooseEnum>("relative_permeability", relperm_model, "The relative permeability model");
  params.addRequiredParam<UserObjectName>("rel_perm_uo", "Name of the User Object defining the relative permeability model");
  params.addRequiredParam<UserObjectName>("cap_pres_uo", "Name of the User Object defining the capillary pressure model");

  params.addRequiredCoupledVar("liquid_saturation","The liquid saturation");
  return params;
}

PorousMaterial::PorousMaterial(const std::string & name,
                                   InputParameters parameters) :
    Material(name, parameters),

    _material_density(getParam<Real>("density")),
    _material_porosity(getParam<Real>("porosity")),
    _material_permeability(getParam<RealTensorValue>("permeability")),
    _material_gravity(getParam<RealVectorValue>("gravity")),
    _material_diffusivity(getParam<Real>("diffusivity")),

    _material_relative_permeability(getUserObject<RelativePermeability>("rel_perm_uo")),
    _material_capillary_pressure(getUserObject<CapillaryPressure>("cap_pres_uo")),

    _rock_density(declareProperty<Real>("density")),
    _porosity(declareProperty<Real>("porosity")),
    _permeability(declareProperty<RealTensorValue>("permeability")),
    _gravity(declareProperty<RealVectorValue>("gravity")),
    _diffusivity(declareProperty<Real>("diffusivity")),
    _gas_relative_permeability(declareProperty<Real>("gas_relative_permeability")),
    _liquid_relative_permeability(declareProperty<Real>("liquid_relative_permeability")),
//  _capillary_pressure(declareProperty<Real>("capillary_pressure")),
    _liquid_saturation(coupledValue("liquid_saturation"))

{}

void
PorousMaterial::computeQpProperties()
{
  // Set material properties at the quadrature points
  _rock_density[_qp] = _material_density;
  _porosity[_qp] = _material_porosity;
  _permeability[_qp] = _material_permeability;
  _gravity[_qp] = _material_gravity;
  _diffusivity[_qp] = _material_diffusivity;
  _gas_relative_permeability[_qp] = _material_relative_permeability.relativePermGas(_liquid_saturation[_qp]);
  _liquid_relative_permeability[_qp] = _material_relative_permeability.relativePermLiq(_liquid_saturation[_qp]);
//  _capillary_pressure[_qp] = _material_capillary_pressure.capillaryPressure(_liquid_saturation[_qp]);
  }
