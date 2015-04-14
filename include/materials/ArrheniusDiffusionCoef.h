#ifndef ARRHENIUSDIFFUSIONCOEF_H
#define ARRHENIUSDIFFUSIONCOEF_H

#include "Material.h"

//Forward Declarations
class ArrheniusDiffusionCoef;

template<>
InputParameters validParams<ArrheniusDiffusionCoef>();

class ArrheniusDiffusionCoef : public Material
{
public:
  ArrheniusDiffusionCoef (const std::string & name,
                InputParameters parameters);

protected:
  virtual void computeQpProperties();

private:

  const Real _d1;
  Function * const _d1_function;
  const VariableValue * const _d1_function_variable;
  const Real _d2;
  const Real _q1;
  const Real _q2;
  const Real _gas_constant;

  const VariableValue & _temp;

  MaterialProperty<Real> & _arrhenius_diffusion_coef;

};

#endif //ARRHENIUSDIFFUSIONCOEF_H
