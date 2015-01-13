#include "ThermalAdvection.h"
#include "Material.h"

template<>
InputParameters validParams<ThermalAdvection>()
{
	InputParameters params = validParams<Kernel>();
	return params;
}

ThermalAdvection::ThermalAdvection(const std::string & name, InputParameters parameters)  :
	Kernel(name, parameters),
	_thermal_advection_coeff(getMaterialProperty<Real>("thermal_advection_coeff")),
	_darcy_velocity(getMaterialProperty<RealGradient>("darcy_velocity"))
{}

Real ThermalAdvection::computeQpResidual()
{
	return _thermal_advection_coeff[_qp]*_test[_i][_qp]*(_darcy_velocity[_qp]*_grad_u[_qp]);
}

Real ThermalAdvection::computeQpJacobian()
{
	return _thermal_advection_coeff[_qp]*_test[_i][_qp]*(_darcy_velocity[_qp]*_grad_phi[_j][_qp]);
}