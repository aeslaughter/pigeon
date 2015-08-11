#ifndef GRAINRADIUSAUX_H
#define GRAINRADIUSAUX_H

#include "AuxKernel.h"

class GrainRadiusAux : public AuxKernel
{

public:

  GrainRadiusAux(const InputParameters & parameters);

  virtual ~GrainRadiusAux() {}

protected:

  virtual Real computeValue();

private:

  const Real conversion_factor;
  const Real gas_constant;

  const VariableValue & _temp;

};

template<>
InputParameters validParams<GrainRadiusAux>();

#endif // GRAINRADIUSAUX_H
