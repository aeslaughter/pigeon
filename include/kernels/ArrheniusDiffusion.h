#ifndef ARRHENIUSDIFFUSION_H
#define ARRHENIUSDIFFUSION_H

#include "Diffusion.h"
#include "Material.h"

//Forward Declarations
class ArrheniusDiffusion;

template<>
InputParameters validParams<ArrheniusDiffusion>();

class ArrheniusDiffusion : public Diffusion
{
public:

  ArrheniusDiffusion(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

private:
  MaterialProperty<Real> & _arrhenius_diffusion_coef;
};

#endif //ARRHENIUSDIFFUSION_H
