#include "Porosity.h"

template<>
InputParameters validParams<Porosity>()
{
  InputParameters params = validParams<ElementAverageValue>();
  return params;
}

Porosity::Porosity(const InputParameters & parameters) :
    ElementAverageValue(parameters)
{
}

Real
Porosity::computeQpIntegral()
{
  Real val = 0.0;

  if (_u[_qp] > 0.5)
    val = 1.0;

  /*if (_t_step < 2)
    return 0.0;
    else*/
  /*Real val = _u[_qp]*_u[_qp];
  if (_u[_qp] < 1.0e-2)
  val = 0.0;*/

  return val;
}
