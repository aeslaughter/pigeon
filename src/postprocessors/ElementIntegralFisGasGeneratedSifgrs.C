#include "ElementIntegralFisGasGeneratedSifgrs.h"

template<>
InputParameters validParams<ElementIntegralFisGasGeneratedSifgrs>()
{
  InputParameters params = validParams<ElementIntegralVariablePostprocessor>();
  return params;
}

ElementIntegralFisGasGeneratedSifgrs::ElementIntegralFisGasGeneratedSifgrs(const std::string & name, InputParameters parameters) :
    ElementIntegralVariablePostprocessor(name, parameters),
    _gas_gen_3(getMaterialProperty<Real>("gas_gen_3"))
{}

Real
ElementIntegralFisGasGeneratedSifgrs::computeQpIntegral()
{
  return _gas_gen_3[_qp];
}
