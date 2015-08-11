#ifndef CALCDIFFUSION_H
#define CALCDIFFUSION_H

#include "Diffusion.h"
#include "Material.h"

class CalcDiffusion;


template<>
InputParameters validParams<CalcDiffusion>();


class CalcDiffusion : public Diffusion
{
public:
 CalcDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  

private:
  MaterialProperty<Real> & _diffusion_coef;
  
}
  ;

#endif //CALCDIFFUSION_H

  

  
