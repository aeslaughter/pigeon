#ifndef RADIOACTIVEDECAYCONSTANT_H
#define RADIOACTIVEDECAYCONSTANT_H

#include "Material.h"


//Forward Declarations
class RadioActiveDecayConstant;

template<>
InputParameters validParams<RadioActiveDecayConstant>();

/**
 * Simple material with constant properties.
 */
class RadioActiveDecayConstant : public Material
{
public:
  RadioActiveDecayConstant(const InputParameters & parameters);

protected:
  virtual void computeProperties();

private:

  Real _my_lambda;

  MaterialProperty<Real> & _lambda;
};

#endif //RADIOACTIVEDECAYCONSTANT_H
