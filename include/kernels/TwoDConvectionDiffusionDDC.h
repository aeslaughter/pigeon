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

#ifndef TWODCONVECTIONDIFFUSIONDDC_H
#define TWODCONVECTIONDIFFUSIONDDC_H

#include "Kernel.h"

class TwoDConvectionDiffusionDDC;

template<>
InputParameters validParams<TwoDConvectionDiffusionDDC>();

class TwoDConvectionDiffusionDDC : public Kernel
{
public:

  TwoDConvectionDiffusionDDC(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  RealTensorValue _gamma;

private:

  VariableGradient & _grad_streamfunction;
};

#endif //TWODCONVECTIONDIFFUSIONDDC_H
