#include "ElementIntegralFisGasGrainSifgrs.h"

template<>
InputParameters validParams<ElementIntegralFisGasGrainSifgrs>()
{
  InputParameters params = validParams<ElementIntegralVariablePostprocessor>();
  return params;
}

ElementIntegralFisGasGrainSifgrs::ElementIntegralFisGasGrainSifgrs(const std::string & name, InputParameters parameters) :
    ElementIntegralVariablePostprocessor(name, parameters),
    _gas_grn_3(getMaterialProperty<Real>("gas_grn_3"))
{}

Real
ElementIntegralFisGasGrainSifgrs::computeQpIntegral()
{
  return _gas_grn_3[_qp];
}
