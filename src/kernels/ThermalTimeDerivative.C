#include "ThermalTimeDerivative.h"
#include "PorousMaterial.h"

template<>
InputParameters validParams<ThermalTimeDerivative>()
{
	InputParameters params = validParams<TimeDerivative>();
	return params;
}

ThermalTimeDerivative::ThermalTimeDerivative(const std::string & name, InputParameters parameters)  :
	TimeDerivative(name, parameters),
	_bulk_specific_heat(getMaterialProperty<Real>("bulk_specific_heat"))
{}

Real
ThermalTimeDerivative::computeQpResidual()
{
	return _bulk_specific_heat[_qp] * TimeDerivative::computeQpResidual();
	//return TimeDerivative::computeQpResidual();
}

Real
ThermalTimeDerivative::computeQpJacobian()
{
	return _bulk_specific_heat[_qp] * TimeDerivative::computeQpJacobian();
	//return TimeDerivative::computeQpJacobian();
}
