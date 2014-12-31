#ifndef CHCHEMPOTENTIAL_H
#define CHCHEMPOTENTIAL_H

#include "CHBulk.h"

//Forward Declarations
class CHChemPotential;

template<>
InputParameters validParams<CHChemPotential>();

/**Cahn-Hilliard Kernel implementing the free energy for sintering
 **/
class CHChemPotential : public CHBulk
{
public:
  CHChemPotential(const std::string & name, InputParameters parameters);

protected:
  virtual RealGradient computeGradDFDCons(PFFunctionType type);
};

#endif //CHMATH_H
