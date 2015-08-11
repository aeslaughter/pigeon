#ifndef POROSITY_H
#define POROSITY_H

#include "ElementAverageValue.h"

//Forward Declarations
class Porosity;

template<>
InputParameters validParams<Porosity>();

/**
 * Compute the porosity (u^2 | u=eta) of the given variable.
 * \todo Probably not what this does.
 */
class Porosity : public ElementAverageValue
{
public:
  Porosity(const InputParameters & parameters);

  virtual Real computeQpIntegral();
};

#endif
