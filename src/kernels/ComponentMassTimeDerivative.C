/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Mass time derivative kernel for multiphase flow in porous    */
/* media for each component.                                    */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "ComponentMassTimeDerivative.h"

template<>
InputParameters validParams<ComponentMassTimeDerivative>()
{
  InputParameters params = validParams<TimeKernel>();
  params.addRequiredCoupledVar("gas_density_variable", "The gas density nodal auxillary variable.");
  params.addRequiredCoupledVar("liquid_density_variable", "The liquid density nodal auxillary variable.");
  params.addRequiredCoupledVar("gas_saturation_variable", "The gas saturation variable.");
  params.addRequiredCoupledVar("liquid_saturation_variable", "The liquid saturation variable.");
  params.addRequiredCoupledVar("xgas_variable", "The component mass fraction in gas variable.");
  params.addRequiredCoupledVar("xliquid_variable", "The component mass fraction in liquid variable.");
  MooseEnum primary_variable_type("pressure, saturation, mass_fraction");
  params.addRequiredParam<MooseEnum>("primary_variable_type", primary_variable_type, "The type of primary variable for this kernel (e.g. pressure, saturation or component mass fraction");
  return params;
}

ComponentMassTimeDerivative::ComponentMassTimeDerivative(const std::string & name,
                       InputParameters parameters) :
    TimeKernel(name, parameters),
    _porosity(getMaterialProperty<Real>("porosity")),
    _gas_density(coupledValue("gas_density_variable")),
    _gas_density_old(coupledValueOld("gas_density_variable")),
    _liquid_density(coupledValue("liquid_density_variable")),
    _liquid_density_old(coupledValueOld("liquid_density_variable")),
    _gas_saturation(coupledNodalValue("gas_saturation_variable")),
    _gas_saturation_old(coupledNodalValueOld("gas_saturation_variable")),
    _liquid_saturation(coupledNodalValue("liquid_saturation_variable")),
    _liquid_saturation_old(coupledNodalValueOld("liquid_saturation_variable")),
    _xgas(coupledNodalValue("xgas_variable")),
    _xgas_old(coupledNodalValueOld("xgas_variable")),
    _xliquid(coupledNodalValue("xliquid_variable")),
    _xliquid_old(coupledNodalValueOld("xliquid_variable")),
    _primary_variable_type(getParam<MooseEnum>("primary_variable_type"))
{}

// Note that this kernel lumps the mass terms to the nodes, so that there is no mass at the qp's.
Real ComponentMassTimeDerivative::computeQpResidual()
{
  Real mass = _porosity[_qp] * (_xgas[_qp] * _gas_density[_qp] * _gas_saturation[_qp] + _xliquid[_qp] * _liquid_density[_qp] *
              _liquid_saturation[_qp]);
  Real mass_old = _porosity[_qp] * (_xgas_old[_qp] * _gas_density_old[_qp] * _gas_saturation_old[_qp] + _xliquid_old[_qp] *
                  _liquid_density_old[_qp] * _liquid_saturation_old[_qp]);

  return _test[_i][_qp] * (mass - mass_old)/_dt;
}

Real ComponentMassTimeDerivative::computeQpJacobian() // TODO: Jacobians need further work!
{
  Real jacobian;

  if (_primary_variable_type == "gas_saturation") {
    jacobian = _test[_i][_qp] * _porosity[_qp] * _xgas[_qp] * _gas_density[_qp] * _phi[_j][_qp]/_dt;
  }

  if (_primary_variable_type == "liquid_saturation") {
    jacobian = _test[_i][_qp] * _porosity[_qp] * _xliquid[_qp] * _liquid_density[_qp] * _phi[_j][_qp]/_dt;
  }

  if (_primary_variable_type == "pressure") {
    jacobian = 0.0;
  }

  if (_primary_variable_type == "mass_fraction") {
    jacobian = _test[_i][_qp] * _porosity[_qp] * _phi[_j][_qp] * (_gas_density[_qp] * _gas_saturation[_qp] +
           _liquid_density[_qp] * _liquid_saturation[_qp])/_dt;
  }

  return jacobian;
}

Real ComponentMassTimeDerivative::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.0;
}
