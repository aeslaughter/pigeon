#ifndef VSWELLINGUO2_H
#define VSWELLINGUO2_H

#include "VolumetricModel.h"

// Forward declarations
class BurnupFunction;

class VSwellingUO2 : public VolumetricModel
{
public:
  VSwellingUO2(const InputParameters & parameters);

  void initStatefulProperties(unsigned n_points);

  virtual void modifyStrain(const unsigned int qp,
                            const Real sclae_factor,
                            SymmTensor & strain_increment,
                            SymmTensor & dstrain_increment_dT);

  static void solidSwelling(const Real solid_swelling_factor,
                            const Real fuel_density,
                            const Real burnup_increment,
                            Real & fract_volumetric,
                            Real & dfract_volumetric_dtemp);

  static void gaseousSwelling(const Real gaseous_swelling_factor,
                              const Real fuel_density,
                              const Real burnup,
                              const Real burnup_increment,
                              const Real temperature,
                              Real & fract_volumetric_increment,
                              Real & dfract_volumetric_increment_dtemp);

  static void densification(const Real total_densification,
                            const Real complete_densification_burnup,
                            const bool constant_c_d,
                            const Real temperature,
                            const Real burnup,
                            Real & fract_volumetric_increment,
                            Real & dfract_volumetric_increment_dtemp);

private:
  const bool _has_burnup;

  VariableValue & _burnup;
  VariableValue & _burnup_old;
  BurnupFunction * const _burnup_function;
  const Real _total_densification;
  const Real _complete_burnup;
  const bool _constant_dens_c_d;
  const Real _density;

  const bool _has_temp;
  VariableValue & _temperature;
  VariableValue & _temperature_old;

  const Real _solid_factor;
  const Real _gas_factor;

  MaterialProperty<Real> * _solid_swell;
  MaterialProperty<Real> * _solid_swell_old;
  MaterialProperty<Real> * _gas_swell;
  MaterialProperty<Real> * _gas_swell_old;
  MaterialProperty<Real> * _densification;
  MaterialProperty<Real> * _densification_old;

};

template<>
InputParameters validParams<VSwellingUO2>();

#endif //VSWELLINGUO2_H
