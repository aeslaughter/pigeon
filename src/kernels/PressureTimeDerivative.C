#include "PressureTimeDerivative.h"
#include "PorousMaterial.h"

template<>
InputParameters validParams<PressureTimeDerivative>()
{
	InputParameters params = validParams<TimeDerivative>();
	return params;
}

PressureTimeDerivative::PressureTimeDerivative(const std::string & name, InputParameters parameters)  :
	TimeDerivative(name, parameters),
	_fluid_transient_coeff(getMaterialProperty<Real>("fluid_transient_coeff"))
{}

Real
PressureTimeDerivative::computeQpResidual()
{
	return _fluid_transient_coeff[_qp] * TimeDerivative::computeQpResidual();
}

Real
PressureTimeDerivative::computeQpJacobian()
{
	return _fluid_transient_coeff[_qp] * TimeDerivative::computeQpJacobian();
}