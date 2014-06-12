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

#include "TwoDDarcyDDC.h"

template<>
InputParameters validParams<TwoDDarcyDDC>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredCoupledVar("concentration_variable", "The gradient of the concentration is passed for the coupling.");
  return params;
}

TwoDDarcyDDC::TwoDDarcyDDC(const std::string & name,
                       InputParameters parameters) :
    Kernel(name, parameters),
    _grad_concentration(coupledGradient("concentration_variable"))
{}

Real TwoDDarcyDDC::computeQpResidual()
{
//  std::cout << _qp << "grad_concentration[1] = " << _grad_concentration[_qp] << "\n";
  return  _test[_i][_qp] * _grad_concentration[_qp](0) - _grad_test[_i][_qp] * _grad_u[_qp];
}

Real TwoDDarcyDDC::computeQpJacobian()
{
  return - _grad_test[_i][_qp] * _grad_phi[_j][_qp];
}
