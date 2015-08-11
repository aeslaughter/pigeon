#ifndef CREEPUPYC_H
#define CREEPUPYC_H

#include "SolidModel.h"

class CreepPyC : public SolidModel
{
public:
  CreepPyC(const InputParameters & params);

protected:

  const Real _densitygcc;
  VariableValue  & _fast_neutron_flux;
  Real _nu;
  Real _K;

  MaterialProperty<SymmTensor> & _creep_strain;
  MaterialProperty<SymmTensor> & _creep_strain_old;

  /// Compute the stress (sigma += deltaSigma)
  virtual void computeStress();
          void computeStressExplicit();

  void computeNuAndK();


private:


};

template<>
InputParameters validParams<CreepPyC>();

#endif //CREEPPYC_H
