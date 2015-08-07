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

#include "Conv2D.h"

/**
 * This function defines the valid parameters for
 * this Kernel and their default values
 */
template<>
InputParameters validParams<Conv2D>()
{
  InputParameters params = validParams<Kernel>();
  params.addParam<Real>("mu",0,"Heat transfer coefficient (m/s).");
  params.addParam<FunctionName>("u_e","0","Function of external temperature (K).");
  return params;
}

Conv2D::Conv2D(const std::string & name,
                                     InputParameters parameters) :
  // You must call the constructor of the base class first
  Kernel(name,parameters),
  _mu(getParam<Real>("mu")),
  _u_e(getFunction("u_e"))
{}

Real Conv2D::computeQpResidual()
{
  const MaterialProperty<Real> & thickness(getMaterialProperty<Real>("thickness"));
  const MaterialProperty<Real> & diffusivity(getMaterialProperty<Real>("diffusivity"));
  Real l = thickness[_qp];
  Real alpha = diffusivity[_qp];
  return 2.0*_mu*l/alpha*(_u[_qp] - _u_e.value(_t,_q_point[_qp]))*_test[_i][_qp];
}

Real Conv2D::computeQpJacobian()
{
  const MaterialProperty<Real> & thickness(getMaterialProperty<Real>("thickness"));
  const MaterialProperty<Real> & diffusivity(getMaterialProperty<Real>("diffusivity"));
  Real l = thickness[_qp];
  Real alpha = diffusivity[_qp];
  return 2*_mu*l/alpha*_phi[_j][_qp]*_test[_i][_qp];
}
