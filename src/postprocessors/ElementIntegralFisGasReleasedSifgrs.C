#include "ElementIntegralFisGasReleasedSifgrs.h"

template<>
InputParameters validParams<ElementIntegralFisGasReleasedSifgrs>()
{
  InputParameters params = validParams<ElementIntegralVariablePostprocessor>();
  return params;
}

ElementIntegralFisGasReleasedSifgrs::ElementIntegralFisGasReleasedSifgrs(const std::string & name, InputParameters parameters) :
    ElementIntegralVariablePostprocessor(name, parameters),
    _gas_rel_3(getMaterialProperty<Real>("gas_rel_3"))
{}

Real
ElementIntegralFisGasReleasedSifgrs::computeQpIntegral()
{
  return _gas_rel_3[_qp];
}
