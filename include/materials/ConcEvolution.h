#ifndef CONCEVOLUTION_H
#define CONCEVOLUTION_H

#include "Material.h"

//Forward Declarations
class ConcEvolution;

template<>
InputParameters validParams<ConcEvolution>();

/**
 * Calculated vacancy and interstitial properties for a given material.  No defaults.
 * The temperature must be in Kelvin
 **/

class ConcEvolution : public Material
{
public:
  ConcEvolution(const InputParameters & parameters);

protected:
  virtual void computeProperties();

private:
  VariableValue & _c;
  VariableGradient & _grad_c;

  VariableValue & _T;

  MaterialProperty<Real> & _kT;
  MaterialProperty<Real> & _M;
  MaterialProperty<Real> & _Ms;
  MaterialProperty<Real> & _D;
  MaterialProperty<RealGradient> & _grad_M;

  MaterialProperty<Real> & _Ef;

  MaterialProperty<Real> & _kappa;
  MaterialProperty<Real> & _c_eq;
  MaterialProperty<Real> & _W;
  MaterialProperty<Real> & _Qstar;

  Real _int_width;
  Real _length_scale;
  Real _time_scale;
  unsigned int _free_energy_form;
  Real _D0;
  Real _Em;
  Real _Efin;
  Real _surface_energy;

  const Real _JtoeV;
  const Real _kb;
};

#endif //CONCEVOLUTION_H
