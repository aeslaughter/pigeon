/*************************************************************************
*
*  Welcome to HYRAX!
*  Andrea M. Jokisaari
*  CASL/MOOSE
*
*  7 November 2013
*
*************************************************************************/

#ifndef CALPHADAB1CD1MATERIAL_H
#define CALPHADAB1CD1MATERIAL_H

#include "CalphadEnergyMaterial.h"
#include "CalphadAB1CD1.h"

class CalphadAB1CD1Material;

template<>
InputParameters validParams<CalphadAB1CD1Material>();

class CalphadAB1CD1Material : public CalphadEnergyMaterial
{
public:
  CalphadAB1CD1Material(const std::string & name, InputParameters parameters);

protected:
  virtual void computeQpProperties();

/*  virtual Real calculateReference(Real c);
  virtual Real calculateIdeal(Real c);
  virtual Real calculateExcess(Real c);

  virtual Real calculateSecondLatticeGminusHser();

  virtual Real computeGMix(Real c);
  virtual Real computeDGMixDc(Real c);
  // virtual Real computeD2GMixDc2();
  //virtual Real computeD3GMixDc3();
  */
private:

  CalphadAB1CD1 _energy;

  MaterialProperty<Real> & _G_AB1CD1;
  MaterialProperty<Real> & _dG_dc;
  MaterialProperty<Real> & _d2G_dc2;
  //MaterialProperty<Real> & _d3G_dc3;
};

#endif //CALPHADAB1CD1MATERIAL_H
