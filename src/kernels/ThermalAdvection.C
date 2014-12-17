#include "ThermalAdvection.h"
#include "Material.h"

template<>
InputParameters validParams<ThermalAdvection>()
{
	InputParameters params = validParams<Kernel>();
	params.addRequiredCoupledVar("pressure_variable", "The pressure gradient is used to calculate the velocity vector");
	return params;
}

ThermalAdvection::ThermalAdvection(const std::string & name, InputParameters parameters)  :
	Kernel(name, parameters),
	_thermal_advection_coeff(getMaterialProperty<Real>("thermal_advection_coeff")),
	_grad_pressure(coupledGradient("pressure_variable"))
{}

Real ThermalAdvection::computeQpResidual()
{
	return _thermal_advection_coeff[_qp]*_test[_i][_qp]*(_grad_pressure[_qp]*_grad_u[_qp]);
}

Real ThermalAdvection::computeQpJacobian()
{
	return _thermal_advection_coeff[_qp]*_test[_i][_qp]*(_grad_pressure[_qp]*_grad_phi[_j][_qp]);
}