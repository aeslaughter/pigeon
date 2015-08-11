#ifndef ThermalFuel_H
#define ThermalFuel_H

#include "Material.h"


//Forward Declarations
class BurnupFunction;

/**
 * Temperature and burnup dependent thermal properties of Uranium Dioxide
 */
class ThermalFuel : public Material
{
public:
  ThermalFuel(const InputParameters & parameters);

protected:
  virtual void computeProperties();
  virtual Real computeSpecificHeat(const Real temp);
  virtual void computeFinkUnirradThCond(const Real temp, Real & cond0, Real & cond0_dt);
  virtual void computeDuriezUnirradThCond(const Real temp, const Real stoich_dev, Real & cond0, Real & cond0_dt);
  virtual void computeAmayaUnirradThCond(const Real temp, const Real Pu_content, Real & cond0, Real & cond0_dt);
  virtual void computeHaldenUnirradThCond(const Real temp, const Real buuo2, const Real stoich_dev, const Real Gd_content, const Real Pu_content, Real & cond0, Real & cond0_dt);
  virtual void computeNFIRUnirradThCond(const Real temp, const Real bu, const Real porosity, Real & cond0, Real & cond0_dt);
  virtual void computeNFImodUnirradThCond(const Real temp, const Real bu, const Real Gd_content, Real & cond0, Real & cond0_dt);
  virtual void computeDissFissProd(const Real temp, const Real bupct, Real & fd, Real & fd_dT);
  virtual void computePrecFissProd(const Real temp1, const Real bupct, Real & fp, Real & fp_dT);
  virtual Real computePor(const Real porosity, const unsigned int qp, const Real cond0);
  virtual void computeRadDamage(const Real temp1, Real & fr, Real & fr_dT);

private:
  bool _has_temp;
  VariableValue  & _temp;
  VariableGradient & _grad_temp;

  bool _has_burnup;
  VariableValue  & _burnup;
  BurnupFunction * const _burnup_function;

  bool _has_porosity;
  VariableValue  & _porosity;

  bool _porosity_material;
  MaterialProperty<Real> * const _porosity_mat;

  MaterialProperty<Real> & _thermal_conductivity;
  MaterialProperty<Real> & _thermal_conductivity_dT;
  MaterialProperty<Real> & _specific_heat;

  Real _initial_porosity;
  Real _stoich_dev;
  Real _Pu_content;
  Real _Gd_content;
  int _model;
};

template<>
InputParameters validParams<ThermalFuel>();

#endif //ThermalFuel_H
