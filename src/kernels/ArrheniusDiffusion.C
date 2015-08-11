#include "ArrheniusDiffusion.h"

template<>
InputParameters validParams<ArrheniusDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addClassDescription("Diffusion with Arrhenius coefficient");
  return params;
}

ArrheniusDiffusion::ArrheniusDiffusion(const InputParameters & parameters) :
  Diffusion(parameters),
  _arrhenius_diffusion_coef(getMaterialProperty<Real>("arrhenius_diffusion_coef"))
{}

Real
ArrheniusDiffusion::computeQpResidual()
{
  return _arrhenius_diffusion_coef[_qp]*Diffusion::computeQpResidual();
}

Real
ArrheniusDiffusion::computeQpJacobian()
{
  return _arrhenius_diffusion_coef[_qp]*Diffusion::computeQpJacobian();
}
