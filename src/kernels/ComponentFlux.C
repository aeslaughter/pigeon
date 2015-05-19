/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Flux kernel for multiphase flow in porous media for each     */
/*  component                                                   */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "ComponentFlux.h"

template<>
InputParameters validParams<ComponentFlux>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredCoupledVar("fluid_density_variable", "The fluid density auxillary variable for the current phase.");
  params.addRequiredCoupledVar("fluid_viscosity_variable", "The fluid viscosity auxillary variable for the current phase.");
  params.addRequiredCoupledVar("fluid_pressure_variable", "The fluid pressure auxillary variable for the current phase.");
  params.addRequiredCoupledVar("xfluid_variable", "The mass fraction of the kernels component in the fluid phase.");
  params.addRequiredCoupledVar("relperm_variable", "The relative permeability of the fluid phase.");
  MooseEnum primary_variable_type("pressure saturation mass_fraction");
  params.addRequiredParam<MooseEnum>("primary_variable_type", primary_variable_type, "The type of primary variable for this kernel (e.g. pressure, saturation or component mass fraction");
  return params;
}

ComponentFlux::ComponentFlux(const std::string & name,
                       InputParameters parameters) :
    Kernel(name, parameters),
    _permeability(getMaterialProperty<RealTensorValue>("permeability")),
    _gravity(getMaterialProperty<RealVectorValue>("gravity")),
    _fluid_density(coupledNodalValue("fluid_density_variable")),
    _fluid_viscosity(coupledNodalValue("fluid_viscosity_variable")),
    _grad_fluid_pressure(coupledGradient("fluid_pressure_variable")),
    _xfluid(coupledNodalValue("xfluid_variable")),
    _fluid_relperm(coupledNodalValue("relperm_variable")),
    _primary_variable_type(getParam<MooseEnum>("primary_variable_type")),
    _fluid_pressure_var(coupled("fluid_pressure_variable"))
{
}

Real ComponentFlux::computeQpResidual()
{
  // The flux without the mobility terms
  return _grad_test[_i][_qp] * (_permeability[_qp] * (_grad_fluid_pressure[_qp] + _fluid_density[_qp] * _gravity[_qp]));
}

void ComponentFlux::computeResidual()
{
  upwind(true, false, 0.);
}
  
Real ComponentFlux::computeQpJacobian() //TODO: Jacobian terms need work!
{
  // Mobility terms not included.
  Real jacobian = 0.0;

  if (_primary_variable_type == "saturation") {
    jacobian = 0.0;
  }

  if (_primary_variable_type == "pressure") {

//  RealTensorValue gas_mobility;

//  gas_mobility = _permeability[_qp] * _gas_relative_permeability[_qp] * _gas_density[_qp] / _gas_viscosity[_qp];
    jacobian = _grad_test[_i][_qp] * (_permeability[_qp] * _grad_phi[_j][_qp]);
  }

  if (_primary_variable_type == "mass_fraction") {

//  RealTensorValue gas_mobility;
//  RealTensorValue liquid_mobility;

//  gas_mobility = _permeability[_qp] * _gas_relative_permeability[_qp] * _gas_density[_qp] / _gas_viscosity[_qp];
//  liquid_mobility = _permeability[_qp] * _liquid_relative_permeability[_qp] * _liquid_density[_qp] / _liquid_viscosity[_qp];

//  RealVectorValue gas_flux = _permeability[_qp] * (_grad_gas_pressure[_qp] - _gas_density[_qp] * _gravity[_qp]);
//  RealVectorValue fluid_flux =  (_grad_liquid_pressure[_qp] - _liquid_density[_qp] * _gravity[_qp]);

    jacobian = _grad_test[_i][_qp] * _phi[_j][_qp] * (_permeability[_qp] * (_grad_fluid_pressure[_qp] + _fluid_density[_qp] * _gravity[_qp]));
  }

  return jacobian;
}

void ComponentFlux::computeJacobian()
{
  upwind(false, true, _var.number());
}

Real ComponentFlux::computeQpOffDiagJacobian(unsigned int jvar)
{
/*
  Real offdiagjacobian;
  RealTensorValue gas_mobility = _permeability[_qp] * _gas_relative_permeability[_qp] * _gas_density[_qp] / _gas_viscosity[_qp];

  if (jvar == _liquid_pressure_var)
    offdiagjacobian = _grad_test[_i][_qp] * (gas_mobility * _grad_phi[_j][_qp]);
  else
    offdiagjacobian = 0.;
*/
  return 0.;
}

void ComponentFlux::upwind(bool compute_res, bool compute_jac, unsigned int jvar)
{
  std::vector<Real> mobility;

  unsigned int num_nodes = _test.size();
  mobility.resize(num_nodes);
  
  // The mobility calculated at the nodes
  for (_i = 0; _i < num_nodes; _i++)
  {
    mobility[_i] = _fluid_relperm[_i] * _fluid_density[_i] / _fluid_viscosity[_i];
  }

  // Compute the residual without the mobility terms (given in computeQpResidual). 
  // Even if we are computing the jacobian we still need this in order to see which 
  // nodes are upwind and which are downwind.
  // Note: this code is taken from kernel.C computeResidual() and computeJacobian()
  DenseVector<Number> & re = _assembly.residualBlock(_var.number());
  _local_re.resize(re.size());
  _local_re.zero();

  for (_i = 0; _i < _test.size(); _i++)
    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
      _local_re(_i) += _JxW[_qp] * _coord[_qp] * computeQpResidual();


  DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), jvar);
  if (compute_jac)
  {
    _local_ke.resize(ke.m(), ke.n());
    _local_ke.zero();

    for (_i = 0; _i < _test.size(); _i++)
      for (_j = 0; _j < _phi.size(); _j++)
        for (_qp = 0; _qp < _qrule->n_points(); _qp++)
          _local_ke(_i, _j) += _JxW[_qp] * _coord[_qp] * computeQpJacobian();
  }


  // Now perform the upwinding by multiplying the residuals at the
  // upstream nodes by their mobilities
  //
  // The residual for the kernel is the darcy flux.
  // This is
  // R_i = int{mobility*flux_no_mob} = int{mobility*grad(pot)*permeability*grad(test_i)}
  // for node i.  where int is the integral over the element.
  // However, in fully-upwind, the first step is to take the mobility outside the
  // integral, which was done in the _local_re calculation above.
  //
  // NOTE: Physically _local_re(_i) is a measure of fluid flowing out of node i
  // If we had left in mobility, it would be exactly the mass flux flowing out of node i.
  //
  // This leads to the definition of upwinding:
  // ***
  // If _local_re(i) is positive then we use mobility_i.  That is
  // we use the upwind value of mobility.
  // ***
  //
  // The final subtle thing is we must also conserve fluid mass: the total mass
  // flowing out of node i must be the sum of the masses flowing
  // into the other nodes.


  // FIRST:
  // this is a dirty way of getting around precision loss problems
  // and problems at steadystate where upwinding oscillates from
  // node-to-node causing nonconvergence.
  bool reached_steady = true;
  for (unsigned int nodenum = 0; nodenum < num_nodes ; ++nodenum)
  {
    if (_local_re(nodenum) >= 1E-20)
    {
      reached_steady = false;
      break;
    }
  }

  // Define variables used to ensure mass conservation
  // total mass out - used for mass conservation
  Real total_mass_out = 0;
  // total flux in
  Real total_in = 0;

  // the following holds derivatives of these
  std::vector<Real> dtotal_mass_out;
  std::vector<Real> dtotal_in;
  if (compute_jac)
  {
    dtotal_mass_out.resize(num_nodes);
    dtotal_in.resize(num_nodes);
    for (unsigned int nodenum = 0; nodenum < num_nodes ; ++nodenum)
    {
      dtotal_mass_out[nodenum] = 0;
      dtotal_in[nodenum] = 0;
    }
  }


  // Perform the upwinding!
  for (unsigned int nodenum = 0; nodenum < num_nodes ; ++nodenum)
  {
    if (_local_re(nodenum) >= 0 || reached_steady) // upstream node
    {
      if (compute_jac)
      {
        for (_j = 0; _j < _phi.size(); _j++)
          _local_ke(nodenum, _j) *= mobility[nodenum];
//      _local_ke(nodenum, nodenum) += _dmobility_dv[nodenum][dvar]*_local_re(nodenum);
        for (_j = 0; _j < _phi.size(); _j++)
          dtotal_mass_out[_j] += _local_ke(nodenum, _j);
      }
      _local_re(nodenum) *= mobility[nodenum];
      total_mass_out += _local_re(nodenum);
    }
    else
    {
      total_in -= _local_re(nodenum); // note the -= means the result is positive
      if (compute_jac)
        for (_j = 0; _j < _phi.size(); _j++)
          dtotal_in[_j] -= _local_ke(nodenum, _j);
    }
  }

  // Conserve mass
  // Proportion the total_mass_out mass to the inflow nodes, weighting by their _local_re values
  if (!reached_steady)
    for (unsigned int nodenum = 0; nodenum < num_nodes ; ++nodenum)
    {
      if (_local_re(nodenum) < 0)
      {
        if (compute_jac)
          for (_j = 0; _j < _phi.size(); _j++)
          {
            _local_ke(nodenum, _j) *= total_mass_out/total_in;
            _local_ke(nodenum, _j) += _local_re(nodenum)*(dtotal_mass_out[_j]/total_in - dtotal_in[_j]*total_mass_out/total_in/total_in);
          }
        _local_re(nodenum) *= total_mass_out/total_in;
      }
    }

  // Add results to the Residual or Jacobian
  if (compute_res)
  {
    re += _local_re;

    if (_has_save_in)
    {
      Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
      for (unsigned int i = 0; i < _save_in.size(); i++)
        _save_in[i]->sys().solution().add_vector(_local_re, _save_in[i]->dofIndices());
    }
  }

  if (compute_jac)  //TODO: needs to be fully implemented
  {
    ke += _local_ke;

//  if (_has_diag_save_in && dvar == _pvar)
    {
      unsigned int rows = ke.m();
      DenseVector<Number> diag(rows);
      for (unsigned int i = 0; i < rows; i++)
        diag(i) = _local_ke(i,i);

      Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
      for (unsigned int i = 0; i < _diag_save_in.size(); i++)
        _diag_save_in[i]->sys().solution().add_vector(diag, _diag_save_in[i]->dofIndices());
    }
  }

}
