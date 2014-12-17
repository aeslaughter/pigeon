#include "ThermalConduction.h"
#include "Material.h"

template<>
InputParameters validParams<ThermalConduction>()
{
	InputParameters params = validParams<Diffusion>();
	return params;
}

ThermalConduction::ThermalConduction(const std::string & name, InputParameters parameters)  :
	Diffusion(name,parameters),
	_bulk_thermal_conductivity(getMaterialProperty<Real>("bulk_thermal_conductivity"))
{}

Real
ThermalConduction::computeQpResidual()
{
	return _bulk_thermal_conductivity[_qp] * Diffusion::computeQpResidual();
}

Real
ThermalConduction::computeQpJacobian()
{
	return _bulk_thermal_conductivity[_qp] * Diffusion::computeQpJacobian();
}