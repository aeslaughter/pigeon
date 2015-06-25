#include "NeutronicSource.h"

template<>
InputParameters validParams<NeutronicSource>() 
{
  InputParameters params = validParams<EigenKernel>(); 
  return params;
}

NeutronicSource::NeutronicSource(const std::string & name, InputParameters parameters) :

  EigenKernel(name,parameters),
  _nu_sigma_f_xs(getMaterialProperty<Real>("nu_sigma_f_xs_g0")) 
{}

Real 
NeutronicSource::computeQpResidual() 
{
return -_nu_sigma_f_xs[_qp]*_test[_i][_qp]*_u[_qp]; 
}

Real 
NeutronicSource::computeQpJacobian() 
{
return -_nu_sigma_f_xs[_qp]*_phi[_j][_qp]*_test[_i][_qp];
}
