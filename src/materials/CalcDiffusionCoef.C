#include "CalcDiffusionCoef.h"

template<>
InputParameters validParams<CalcDiffusionCoef>()
{
  InputParameters params = validParams<Material>();
  params.addRequiredCoupledVar("temp", "Coupled Temperature");
  params.addParam<Real>("D0", "Initial Diffusion Coefficient (m^2/sec)");
  params.addParam<Real>("Q", "Activation Energy (J/mol)");
  params.addParam<Real>("R", "Universal Gas Constant (J/mol-K)");
  params.addParam<Real>("diffusion_coef", "Diffusion Coefficient");
  
  
  return params;
}

CalcDiffusionCoef::CalcDiffusionCoef(const InputParameters & parameters)
    : Material(parameters),
      _D0(getParam<Real>("D0")),
      _Q(getParam<Real>("Q")),
      _R(getParam<Real>("R")),

      _temp(coupledValue("temp")),

      _diffusion_coef(declareProperty<Real>("diffusion_coef"))

{

}


void
CalcDiffusionCoef::computeProperties()
{

  if (_temp[_qp] > 0)
  {
    _diffusion_coef[_qp] = _D0 * std::exp(-_Q/(_R*_temp[_qp]));

   
  }
  else
  {
    _diffusion_coef[_qp] = 0;
    }

}
