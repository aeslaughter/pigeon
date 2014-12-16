#include "ThermalAdvection.h"
#include "PorousMaterial.h"

template<>
InputParameters validParams<ThermalAdvection>()
{
	InputParameters params = validParams<Kernel>();
	params.addRequiredCoupledVar("pressure", "The pressure gradient is used to calculate the velocity vector");
	return params;
}

ThermalAdvection::ThermalAdvection(const std::string & name, InputParameters parameters)  :
	Kernel(name, parameters),
	_fluid_specific_heat(getMaterialProperty<Real>("fluid_specific_heat")),
	_grad_pressure(coupledGradient("pressure"))
{}

Real ThermalAdvection::computeQpResidual()
{
	return _fluid_specific_heat[_qp]*_test[_i][_qp]*(_grad_pressure[_qp]*_grad_u[_qp]);
}

Real ThermalAdvection::computeQpJacobian()
{
	return _fluid_specific_heat[_qp]*_test[_i][_qp]*(_grad_pressure[_qp]*_grad_phi[_j][_qp]);
}