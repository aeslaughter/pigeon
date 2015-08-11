#ifndef FASTNEUTRONFLUXAUX_H
#define FASTNEUTRONFLUXAUX_H

#include "AuxKernel.h"
#include "Function.h"

//Forward Declarations
class FastNeutronFluxAux;

template<>
InputParameters validParams<FastNeutronFluxAux>();

/**
 * Coupled auxiliary value
 */
class FastNeutronFluxAux : public AuxKernel
{
public:

  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  FastNeutronFluxAux(const InputParameters & parameters);

  virtual ~FastNeutronFluxAux() {}

protected:
  virtual Real computeValue();

private:
  const Real _factor;
  Function * const _function;
  Function * const _ralp;
  Function * const _axial_profile;
  const VariableValue * _q_variable;
};

#endif //FASTNEUTRONFLUXAUX_H
