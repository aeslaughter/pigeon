/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef CONSTANTMASSFLUXBC_H
#define CONSTANTMASSFLUXBC_H

#include "IntegratedBC.h"

class ConstantMassFluxBC;

template<>
InputParameters validParams<ConstantMassFluxBC>();

class ConstantMassFluxBC : public IntegratedBC
{
public:

  ConstantMassFluxBC(const std::string & name,
            InputParameters parameters);

private:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  Real _value;
  const MaterialProperty<RealVectorValue> & _gravity;
  VariableValue & _fluid_density;

};

#endif //CONSTANTMASSFLUXBC_H
