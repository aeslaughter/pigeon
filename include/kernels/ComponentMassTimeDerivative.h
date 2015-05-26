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
#include "FluidState.h"

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
  std::vector<VariableValue *> _fluid_density;
  std::vector<VariableValue *> _fluid_density_old;
  std::vector<VariableValue *> _fluid_saturation;
  std::vector<VariableValue *> _fluid_saturation_old;
  std::vector<VariableValue *> _component_mass_fraction;
  std::vector<VariableValue *> _component_mass_fraction_old;
  std::vector<VariableValue *> _fluid_pressure;
  const MooseEnum & _primary_variable_type;
  VariableValue & _temperature; 

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;
  unsigned int _num_phases;
};

#endif //COMPONENTMASSTIMEDERIVATIVE_H
