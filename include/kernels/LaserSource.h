/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef LASERSOURCE_H
#define LASERSOURCE_H

#include "Kernel.h"
#include "Function.h"

/**
 * The forward declaration is so that we can declare the validParams() function
 * before we actually define the class... that way the definition isn't lost
 * at the bottom of the file.
 */

// Forward Declarations
class LaserSource;

/**
 * validParams returns the parameters that this Kernel accepts / needs
 * The actual body of the function MUST be in the .C file.
 */
template<>
InputParameters validParams<LaserSource>();

/**
 * Define the Kernel for the laser source that looks like:
 *
 * (1/(p_c*rho)*exp(-d(x,x0)/sigma),test)
 */
class LaserSource : public Kernel
{
public:

  /**
   * This is the constructor declaration.  This class takes a
   * string and a InputParameters object, just like other
   * Kernel-derived classes.
   */
  LaserSource(const std::string & name,
                    InputParameters parameters);

protected:
  /**
   * Responsible for computing the residual at one quadrature point.
   * This function should always be defined in the .C file.
   */
  virtual Real computeQpResidual();

private:
  
  // Constants
  Function & _power;
  Function & _x0;
  Function & _y0;
  Function & _sigma;

};

#endif // LASERSOURCE_H
