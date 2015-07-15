/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "ConstantMassFluxBC.h"

template<>
InputParameters validParams<ConstantMassFluxBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addParam<Real>("value", 0.0,  "The value of the mass flux at the boundary");
  params.addRequiredCoupledVar("fluid_density_variable", "The fluid density auxillary variable");
  return params;
}

ConstantMassFluxBC::ConstantMassFluxBC(const std::string & name, InputParameters params) :
    IntegratedBC(name, params),
    _value(getParam<Real>("value")),
    _gravity(getMaterialProperty<RealVectorValue>("gravity")),
    _fluid_density(coupledValue("fluid_density_variable"))
{
}

Real
ConstantMassFluxBC::computeQpResidual()
{
  return  _test[_i][_qp] * _value; // - _fluid_density[_qp] * _gravity[_qp] * _normals[_qp]);
}

Real
ConstantMassFluxBC::computeQpJacobian()
{
  return 0.0;
}
