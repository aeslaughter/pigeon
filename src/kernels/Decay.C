#include "Decay.h"
#include "Material.h"

template<>
InputParameters validParams<Decay>()
{
  InputParameters params = validParams<Kernel>();
  return params;
}

Decay::Decay(const InputParameters & parameters)
  :Kernel(parameters),
   _lambda(getMaterialProperty<Real>("lambda"))
{}

Real
Decay::computeQpResidual()
{
  return _lambda[_qp]*_test[_i][_qp]*_u[_qp];
}

Real
Decay::computeQpJacobian()
{
  return _lambda[_qp]*_test[_i][_qp]*_phi[_j][_qp];
}

