#include "CoupledHeatSource.h"

template<>
InputParameters validParams<CoupledHeatSource>()
{
  InputParameters params = validParams<Kernel>();

  // Override defaults and documentation, weak form is identical to BodyForce in MOOSE
  params.addRequiredCoupledVar("powerDensity","Power density variable");
  params.addRequiredParam<Real>("scaledPower","Scaling coefficient: Total Power / SerpentNorm");
  return params;
}

CoupledHeatSource::CoupledHeatSource(const std::string & name, InputParameters parameters)
  :Kernel(name, parameters),
  _powerDensity(coupledValue("powerDensity")),
  _scaledPower(getParam<Real>("scaledPower"))
{}

Real
CoupledHeatSource::computeQpResidual()
{
  return -_test[_i][_qp]*_scaledPower*_powerDensity[_qp];
}

Real
CoupledHeatSource::computeQpJacobian()
{
  return 0;
}
