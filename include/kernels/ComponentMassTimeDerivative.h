/****************************************************************/
/* Mass time derivative kernel for multiphase flow in porous    */
/* media for each component.                                    */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef COMPONENTMASSTIMEDERIVATIVE_H
#define COMPONENTMASSTIMEDERIVATIVE_H

#include "TimeKernel.h"

class ComponentMassTimeDerivative;

template<>
InputParameters validParams<ComponentMassTimeDerivative>();

class ComponentMassTimeDerivative : public TimeKernel
{
public:
  ComponentMassTimeDerivative(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // Member reference to the porosity material property
  MaterialProperty<Real> & _porosity;


private:
  VariableValue & _gas_density;
  VariableValue & _gas_density_old;
  VariableValue & _liquid_density;
  VariableValue & _liquid_density_old;
  VariableValue & _gas_saturation;
  VariableValue & _gas_saturation_old;
  VariableValue & _liquid_saturation;
  VariableValue & _liquid_saturation_old;
  VariableValue & _xgas;
  VariableValue & _xgas_old;
  VariableValue & _xliquid;
  VariableValue & _xliquid_old;
  const MooseEnum & _primary_variable_type;
};

#endif //COMPONENTMASSTIMEDERIVATIVE_H
