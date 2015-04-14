#ifndef CALCDIFFUSIONCOEF_H
#define CALCDIFFUSIONCOEF_H

#include "Material.h"

class CalcDiffusionCoef;


template<>
InputParameters validParams<CalcDiffusionCoef>();


class CalcDiffusionCoef : public Material

{
public:
  CalcDiffusionCoef (const std::string & name, InputParameters parameters);

protected:
  virtual void computeProperties();
  
private:
  const Real _D0;
  const Real _Q;
  const Real _R;

  const VariableValue & _temp;

  MaterialProperty<Real> & _diffusion_coef;

}
  ;

#endif //CALCDIFFUSIONCOEF_H

  

  
