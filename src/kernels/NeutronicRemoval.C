#include "NeutronicRemoval.h"

template<>
InputParameters validParams<NeutronicRemoval>() 
{
  InputParameters params = validParams<Reaction>(); 
  return params;
}

NeutronicRemoval::NeutronicRemoval(const InputParameters & parameters) :
  Reaction(parameters),
  _absorption_xs(getMaterialProperty<Real>("absorption_xs_g0")) 
{}

Real 
NeutronicRemoval::computeQpResidual() 
{
  return _absorption_xs[_qp]*Reaction::computeQpResidual(); 
}

Real 
NeutronicRemoval::computeQpJacobian() 
{
  return _absorption_xs[_qp]*Reaction::computeQpJacobian();
}
