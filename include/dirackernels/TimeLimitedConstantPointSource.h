/****************************************************************/
/* Constant point source Dirac kernel with optional temporal    */
/* limit so that the source can be limited to a specified time  */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef TIMELIMITEDCONSTANTPOINTSOURCE_H
#define TIMELIMITEDCONSTANTPOINTSOURCE_H

// Moose Includes
#include "DiracKernel.h"

//Forward Declarations
class TimeLimitedConstantPointSource;

template<>
InputParameters validParams<TimeLimitedConstantPointSource>();

/**
 * TOOD
 */
class TimeLimitedConstantPointSource : public DiracKernel
{
public:
  TimeLimitedConstantPointSource(const std::string & name, InputParameters parameters);

  virtual void addPoints();
  virtual Real computeQpResidual();

protected:
  Real _value;
  std::vector<Real> _point_param;
  Point _p;
  Real _end_time;
};

#endif //TIMELIMITEDCONSTANTPOINTSOURCE_H
