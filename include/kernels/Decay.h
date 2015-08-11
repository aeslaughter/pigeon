#ifndef DECAY_H
#define DECAY_H

#include "Kernel.h"

//Forward Declarations
class Decay;

template<>
InputParameters validParams<Decay>();

class Decay : public Kernel
{
public:

  Decay(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

private:
  MaterialProperty<Real> & _lambda;
};

#endif //DECAY_H
