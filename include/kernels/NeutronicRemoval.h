#ifndef NEUTRONICREMOVAL_H 
#define NEUTRONICREMOVAL_H

#include "Reaction.h"

//Forward Declarations 
class NeutronicRemoval;

template<>
InputParameters validParams<NeutronicRemoval>();

class NeutronicRemoval : public Reaction 
{
public:

  NeutronicRemoval(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual(); 
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _absorption_xs; 
};
#endif //NEUTRONICREMOVAL_H
