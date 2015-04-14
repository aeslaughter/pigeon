#ifndef FASTNEUTRONFLUENCEAUX_H
#define FASTNEUTRONFLUENCEAUX_H

#include "AuxKernel.h"

//Forward Declarations
class FastNeutronFluenceAux;

template<>
InputParameters validParams<FastNeutronFluenceAux>();

/**
 * Coupled auxiliary value
 */
class FastNeutronFluenceAux : public AuxKernel
{
public:

  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  FastNeutronFluenceAux(const std::string & name, InputParameters parameters);

  virtual ~FastNeutronFluenceAux() {}

protected:
  virtual Real computeValue();

private:
  VariableValue & _fast_neutron_flux;
  VariableValue & _fast_neutron_flux_old;
};

#endif //FASTNEUTRONFLUENCEAUX_H
