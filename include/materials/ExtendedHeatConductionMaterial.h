#ifndef EXTENDEDHEATCONDUCTIONMATERIAL_H
#define EXTENDEDHEATCONDUCTIONMATERIAL_H

#include "HeatConductionMaterial.h"

//Extends HeatConductionMaterial to have a density property that can depend on temp
//Density.h is an alternative to this class that has ability to describe density based on expansion which would be temeprature dependent. This class is a  way to describe \rho(T) by neglecting thermal expansion

//Forward Declarations
class ExtendedHeatConductionMaterial;
class Function;

template<>
InputParameters validParams<ExtendedHeatConductionMaterial>();

class ExtendedHeatConductionMaterial : public HeatConductionMaterial
{
public:
  ExtendedHeatConductionMaterial(const std::string & name,
                         InputParameters parameters);

protected:
  virtual void computeProperties();
  
  const Real _my_density;
  MaterialProperty<Real> & _density;
  Function * const _density_function;
};

#endif //ExtendedHeatConductionMaterial_H
