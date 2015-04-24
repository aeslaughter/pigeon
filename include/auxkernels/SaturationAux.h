/****************************************************************/
/* Auxillary kernel to calculate saturation for multiphase flow */
/* in porous media.                                             */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef SATURATIONAUX_H
#define SATURATIONAUX_H

#include "AuxKernel.h"

class SaturationAux;

template<>
InputParameters validParams<SaturationAux>();

class SaturationAux : public AuxKernel
{
public:

  SaturationAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  VariableValue & _saturation;
};

#endif //SATURATIONAUX_H
