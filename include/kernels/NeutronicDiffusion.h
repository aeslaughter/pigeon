#ifndef NEUTRONICDIFFUSION_H 
#define NEUTRONICDIFFUSION_H

#include "Diffusion.h"

//Forward Declarations 
class NeutronicDiffusion;

template<>
InputParameters validParams<NeutronicDiffusion>();

class NeutronicDiffusion : public Diffusion 
{
public:

  NeutronicDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual(); 
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _diffusion_coef; 
};
#endif //NEUTRONICDIFFUSION_H
