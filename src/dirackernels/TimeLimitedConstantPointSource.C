/****************************************************************/
/* Constant point source Dirac kernel with optional temporal    */
/* limit so that the source can be limited to a specified time  */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "TimeLimitedConstantPointSource.h"

template<>
InputParameters validParams<TimeLimitedConstantPointSource>()
{
  InputParameters params = validParams<DiracKernel>();
  params.addRequiredParam<Real>("value", "The value of the point source");
  params.addRequiredParam<std::vector<Real> >("point", "The x,y,z coordinates of the point");
  params.addParam<Real>("end_time", 1.0e30, "The time at which the source will end.");
  return params;
}

TimeLimitedConstantPointSource::TimeLimitedConstantPointSource(const std::string & name, InputParameters parameters) :
    DiracKernel(name, parameters),
    _value(getParam<Real>("value")),
    _point_param(getParam<std::vector<Real> >("point")),
    _end_time(getParam<Real>("end_time"))
{
  _p(0) = _point_param[0];

  if (_point_param.size() > 1)
  {
    _p(1) = _point_param[1];

    if (_point_param.size() > 2)
    {
      _p(2) = _point_param[2];
    }
  }
}

void
TimeLimitedConstantPointSource::addPoints()
{
  addPoint(_p);
}

Real
TimeLimitedConstantPointSource::computeQpResidual()
{
  if (_t <= _end_time) // TODO: Want to make sure that _t hits end_time exactly.
    return -_test[_i][_qp]*_value;
  else
    return 0.0;
}

