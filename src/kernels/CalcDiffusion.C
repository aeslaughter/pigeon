#include "CalcDiffusion.h"

template<>
InputParameters validParams<CalcDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addClassDescription("Diffusion Coeffiecient calculation");
  return params;
}

CalcDiffusion::CalcDiffusion(const InputParameters & parameters)
    : Diffusion(parameters),
      _diffusion_coef(getMaterialProperty<Real>("diffusion_coef"))
{

}


Real
CalcDiffusion::computeQpResidual()
{

  Real diffusion_coef(0);
  
  diffusion_coef = _diffusion_coef[_qp]*Diffusion::computeQpResidual();
  return diffusion_coef;
  
}                       
    
Real
CalcDiffusion::computeQpJacobian()
{
  return _diffusion_coef[_qp]*Diffusion::computeQpJacobian();
}
