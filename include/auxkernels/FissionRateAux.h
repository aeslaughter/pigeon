/*************************************************/
/*           DO NOT MODIFY THIS HEADER           */
/*                                               */
/*                     BISON                     */
/*                                               */
/*    (c) 2014 Battelle Energy Alliance, LLC     */
/*            ALL RIGHTS RESERVED                */
/*                                               */
/*   Prepared by Battelle Energy Alliance, LLC   */
/*     Under Contract No. DE-AC07-05ID14517      */
/*     With the U. S. Department of Energy       */
/*                                               */
/*     See COPYRIGHT for full restrictions       */
/*************************************************/

#ifndef FISSIONRATEAUX_H
#define FISSIONRATEAUX_H

#include "AuxKernel.h"


//Forward Declarations
class FissionRateAux;

template<>
InputParameters validParams<FissionRateAux>();

/**
 * Coupled auxiliary value
 */
class FissionRateAux : public AuxKernel
{
public:

  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  FissionRateAux(const std::string & name, InputParameters parameters);

  virtual ~FissionRateAux() {}

protected:
  virtual Real computeValue();

  const Real _value;

  const bool _has_function;
  Function * _function;
};

#endif //FISSIONRATEAUX_H
