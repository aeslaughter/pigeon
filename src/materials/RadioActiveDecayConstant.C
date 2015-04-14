#include "RadioActiveDecayConstant.h"

template<>
InputParameters validParams<RadioActiveDecayConstant>()
{
  InputParameters params = validParams<Material>();

  params.addRequiredParam<Real>("lambda", "The radio active decay constant");
return params;
}

RadioActiveDecayConstant::RadioActiveDecayConstant(const std::string & name,
                   InputParameters parameters)
  :Material(name, parameters),
   _my_lambda(getParam<Real>("lambda")),
   _lambda(declareProperty<Real>("lambda"))
  {}

void
RadioActiveDecayConstant::computeProperties()
{
  for(unsigned int qp=0; qp<_qrule->n_points(); qp++)
  {
    _lambda[qp] = _my_lambda;
  }
}
