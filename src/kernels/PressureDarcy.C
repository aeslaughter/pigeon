#include "PressureDarcy.h"
#include "PorousMaterial.h"

template<>
InputParameters validParams<PressureDarcy>()
{
	InputParameters params = validParams<Diffusion>();
	return params;
}

PressureDarcy::PressureDarcy(const std::string & name, InputParameters parameters)  :
	Diffusion(name, parameters),
	_hydraulic_conductivity(getMaterialProperty<Real>("hydraulic_conductivity"))
{}

Real
PressureDarcy::computeQpResidual()
{
	return _hydraulic_conductivity[_qp] * Diffusion::computeQpResidual();
}

Real
PressureDarcy::computeQpJacobian()
{
	return _hydraulic_conductivity[_qp] * Diffusion::computeQpJacobian();
}