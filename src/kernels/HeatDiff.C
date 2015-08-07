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

#include "HeatDiff.h"

/**
 * This function defines the valid parameters for
 * this Kernel and their default values
 */
template<>
InputParameters validParams<HeatDiff>()
{
  InputParameters params = validParams<Kernel>();
  return params;
}

HeatDiff::HeatDiff(const std::string & name,
                                     InputParameters parameters) :
  // You must call the constructor of the base class first
  Kernel(name,parameters)
{}

Real HeatDiff::computeQpResidual()
{
  const MaterialProperty<Real> & thickness(getMaterialProperty<Real>("thickness"));
  const MaterialProperty<Real> & diffusivity(getMaterialProperty<Real>("diffusivity"));
  Real l = thickness[_qp];
  Real alpha = diffusivity[_qp];
  return alpha/l*_grad_u[_qp]*_grad_test[_i][_qp];
}

Real HeatDiff::computeQpJacobian()
{
  const MaterialProperty<Real> & thickness(getMaterialProperty<Real>("thickness"));
  const MaterialProperty<Real> & diffusivity(getMaterialProperty<Real>("diffusivity"));
  Real l = thickness[_qp];
  Real alpha = diffusivity[_qp];
  return alpha/l*_grad_phi[_j][_qp]*_grad_test[_i][_qp];
}
