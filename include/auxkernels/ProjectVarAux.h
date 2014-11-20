#ifndef PROJECTVARAUX_H
#define PROJECTVARAUX_H

#include "AuxKernel.h"


//Forward Declarations
class ProjectVarAux;

template<>
InputParameters validParams<ProjectVarAux>();

/**
 * Coupled auxiliary value
 */
class ProjectVarAux : public AuxKernel
{
public:

  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  ProjectVarAux(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeValue();

  VariableValue & _coupled_val;

  Real _value;
};

#endif //ProjectVarAux_H
