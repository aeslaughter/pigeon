#ifndef DIFFUSIONNEUTRONICSMATERIAL_H 
#define DIFFUSIONNEUTRONICSMATERIAL_H

#include "Material.h"

//Forward Declarations

class DiffusionNeutronicsMaterial;

template<>
InputParameters validParams<DiffusionNeutronicsMaterial>();

class DiffusionNeutronicsMaterial : public Material 
{
public:

  DiffusionNeutronicsMaterial(const std::string & name, InputParameters parameters);

protected:
  virtual void computeQpProperties();

  Real _diffusion_coef_value;
  Real _absorption_value;
  Real _nu_sigma_f_value; 
  MaterialProperty<Real> & _diffusion_coef; 
  MaterialProperty<Real> & _absorption_xs; 
  MaterialProperty<Real> & _nu_sigma_f_xs;
};
#endif //DIFFUSIONNEUTRONICSMATERIAL_H
