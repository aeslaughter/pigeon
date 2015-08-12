#include "DiffusionNeutronicsMaterial.h"

template<>
InputParameters validParams<DiffusionNeutronicsMaterial>() 
{
  InputParameters params = validParams<Material>(); 
  params.addParam<Real>("diffusion_coef",1.0,"The diffusion coefficient"); 
  params.addParam<Real>("sigma_abs",1.0,"The absorption coefficient"); 
  params.addParam<Real>("nusigma_f",1.0,"The multiplication coefficient"); 
  return params;
}

DiffusionNeutronicsMaterial::DiffusionNeutronicsMaterial(const InputParameters & parameters) : 
  Material(parameters),
  _diffusion_coef_value(getParam<Real>("diffusion_coef")),
  _absorption_value(getParam<Real>("sigma_abs")),
  _nu_sigma_f_value(getParam<Real>("nusigma_f")), 
  _diffusion_coef(declareProperty<Real>("diffusion_coef_g0")), 
  _absorption_xs(declareProperty<Real>("absorption_xs_g0")), 
  _nu_sigma_f_xs(declareProperty<Real>("nu_sigma_f_xs_g0"))
{ 
}

void 
DiffusionNeutronicsMaterial::computeQpProperties() 
{
  _diffusion_coef[_qp] = _diffusion_coef_value; 
  _absorption_xs[_qp] = _absorption_value; 
  _nu_sigma_f_xs[_qp] = _nu_sigma_f_value;
}
