/****************************************************************/
/* Boundary condition to set the mass flux equal to a constant  */
/* value over a side.                                           */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
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
  MaterialProperty<RealVectorValue> & _gravity;
  VariableValue & _fluid_density;

};

#endif //CONSTANTMASSFLUXBC_H
