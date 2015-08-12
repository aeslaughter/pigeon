#include "NeutronicDiffusion.h"

template<>
InputParameters validParams<NeutronicDiffusion>() 
{
  InputParameters params = validParams<Diffusion>(); 
  return params;
}

NeutronicDiffusion::NeutronicDiffusion(const InputParameters & parameters) :
  Diffusion(parameters),
  _diffusion_coef(getMaterialProperty<Real>("diffusion_coef_g0")) 
{}

Real 
NeutronicDiffusion::computeQpResidual() 
{
  return _diffusion_coef[_qp]*Diffusion::computeQpResidual(); 
}

Real 
NeutronicDiffusion::computeQpJacobian() 
{
  return _diffusion_coef[_qp]*Diffusion::computeQpJacobian();
}
