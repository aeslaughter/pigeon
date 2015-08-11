#include "ArrheniusDiffusionCoef.h"

/* Computes a two-term Arrhenius diffusion coeffient of the form:

   d = d1*exp(-q1/(RT)) + d2*exp(-q2/(RT))

*/

template<>
InputParameters validParams<ArrheniusDiffusionCoef>()
{
  InputParameters params = validParams<Material>();

  // Default material constants are for Cesium diffusion in UO2
  params.addParam<Real>("d1", 5.6e-8, "First coefficient (m**2/s)");
  params.addParam<FunctionName>("d1_function", "Function to be multiplied by d1");
  params.addCoupledVar("d1_function_variable", "Variable to be used when evaluating d1_function. If not given, time will be used.");
  params.addParam<Real>("d2", 5.2e-4, "Second coefficient (m**2/s)");
  params.addParam<Real>("q1", 2.09e5, "First activation energy (J/mol)");
  params.addParam<Real>("q2", 3.62e5, "Second activation energy (J/mol)");
  params.addParam<Real>("gas_constant", 8.3143, "Universal gas constant (J/mol-K)");

  params.addCoupledVar("temp", "Coupled Temp");

  return params;
}

ArrheniusDiffusionCoef::ArrheniusDiffusionCoef(const InputParameters & parameters)
  :Material(parameters),

   _d1(getParam<Real>("d1")),
   _d1_function(isParamValid("d1_function") ? &getFunction("d1_function") : NULL),
   _d1_function_variable(isCoupled("d1_function_variable") ? &coupledValue("d1_function_variable") : NULL),
   _d2(getParam<Real>("d2")),
   _q1(getParam<Real>("q1")),
   _q2(getParam<Real>("q2")),
   _gas_constant(getParam<Real>("gas_constant")),

   _temp(coupledValue("temp")),

   _arrhenius_diffusion_coef(declareProperty<Real>("arrhenius_diffusion_coef"))
{

}

void
ArrheniusDiffusionCoef::computeQpProperties()
{
  if (_temp[_qp] > 0)
  {
    Real d1 = _d1;
    if (_d1_function)
    {
      if (_d1_function_variable)
      {
        d1 *= _d1_function->value((*_d1_function_variable)[_qp], _q_point[_qp]);
      }
      else
      {
        d1 *= _d1_function->value(_t, _q_point[_qp]);
      }
    }
    _arrhenius_diffusion_coef[_qp] = d1*std::exp(-_q1/(_gas_constant*_temp[_qp])) + _d2*std::exp(-_q2/(_gas_constant*_temp[_qp]));
  }
  else
  {
    _arrhenius_diffusion_coef[_qp] = 0;
  }
}
