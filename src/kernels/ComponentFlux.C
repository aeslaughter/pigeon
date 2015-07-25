/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "ComponentFlux.h"

template<>
InputParameters validParams<ComponentFlux>()
{
  InputParameters params = validParams<Kernel>();
//  params.addRequiredCoupledVar("fluid_pressure_variable", "The fluid pressure variable for this phase");
//  params.addRequiredCoupledVar("fluid_saturation_variable", "The fluid saturation variable for this phase");
//  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
//  params.addRequiredCoupledVar("fluid_density_variable", "The fluid density auxiallary variable for this phase.");
//  params.addRequiredCoupledVar("fluid_viscosity_variable", "The  fluid viscosity auxillary variable for this phase.");
  params.addRequiredCoupledVar("component_mass_fraction_variable", "The mass fraction of the kernels component in this phase.");
//  params.addRequiredCoupledVar("fluid_relperm_variable", "The relative permeability auxillary variable for this phase.");
  params.addParam<unsigned int>("phase_index", 0, "The index corresponding to the fluid phase eg: 0 for liquid, 1 for gas");
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  return params;
}

ComponentFlux::ComponentFlux(const std::string & name,
                       InputParameters parameters) :
  Kernel(name, parameters),
  _permeability(getMaterialProperty<RealTensorValue>("permeability")),
  _gravity(getMaterialProperty<RealVectorValue>("gravity")),
  _phase_flux_no_mobility(getMaterialProperty<std::vector<RealGradient> >("phase_flux_no_mobility")),
  _dgravity_flux_dp(getMaterialProperty<std::vector<RealVectorValue> >("dgravity_flux_dp")),
  _dgravity_flux_ds(getMaterialProperty<std::vector<RealVectorValue> >("dgravity_flux_ds")),
  _dpressure_flux_dp(getMaterialProperty<std::vector<Real> >("dpressure_flux_dp")),
  _dpressure_flux_ds(getMaterialProperty<std::vector<Real> >("dpressure_flux_ds")),
//  _fluid_pressure(coupledNodalValue("fluid_pressure_variable")),
//  _fluid_saturation(coupledNodalValue("fluid_saturation_variable")),
//  _temperature(coupledNodalValue("temperature_variable")),
//  _fluid_density(coupledNodalValue("fluid_density_variable")),
//  _fluid_viscosity(coupledNodalValue("fluid_viscosity_variable")),
  _component_mass_fraction(coupledNodalValue("component_mass_fraction_variable")),
//  _fluid_relperm(coupledNodalValue("fluid_relperm_variable")),
  _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
  _phase_index(getParam<unsigned int>("phase_index"))

{
  if (!_fluid_state.isFluidStateVariable(_var.number()))
    mooseError("Variable " << _var.name() << " in the " << _name << " kernel is not a FluidState variable");

  // Determine the primary variable type
  _primary_variable_type = _fluid_state.variableTypes(_var.number());

}

Real ComponentFlux::computeQpResidual()
{
  return _grad_test[_i][_qp] * (_permeability[_qp] * _phase_flux_no_mobility[_qp][_phase_index]);
}

void ComponentFlux::computeResidual()
{
  upwind(true, false, 0.);
}

Real ComponentFlux::computeQpJacobian()
{
  Real qpjacobian;

  if (_primary_variable_type == "pressure")
    qpjacobian = _grad_test[_i][_qp] * (_permeability[_qp] * (_grad_phi[_j][_qp] * _dpressure_flux_dp[_qp][_phase_index] + _phi[_j][_qp] * _dgravity_flux_dp[_qp][_phase_index]));

  if (_primary_variable_type == "saturation")
    qpjacobian = _grad_test[_i][_qp] * (_permeability[_qp] * (_grad_phi[_j][_qp] * _dpressure_flux_ds[_qp][_phase_index] + _phi[_j][_qp] * _dgravity_flux_ds[_qp][_phase_index]));

  if (_primary_variable_type == "mass_fraction")
    qpjacobian = 0.; //TODO

  return qpjacobian;
}

void ComponentFlux::computeJacobian()
{
   upwind(false, true, _var.number());
}

Real ComponentFlux::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (!_fluid_state.isFluidStateVariable(jvar))
    return 0.0;

  Real qpoffdiagjacobian;

  // Determine the variable type to take the derivative with respect to
  std::string jvar_type = _fluid_state.variableTypes(jvar);

  if (jvar_type == "pressure")
    qpoffdiagjacobian = _grad_test[_i][_qp] * (_permeability[_qp] * (_grad_phi[_j][_qp] * _dpressure_flux_dp[_qp][_phase_index] + _phi[_j][_qp] * _dgravity_flux_dp[_qp][_phase_index]));

  if (jvar_type == "saturation")
    qpoffdiagjacobian = _grad_test[_i][_qp] * (_permeability[_qp] * (_grad_phi[_j][_qp] * _dpressure_flux_ds[_qp][_phase_index] + _phi[_j][_qp] * _dgravity_flux_ds[_qp][_phase_index]));

  if (jvar_type == "mass_fraction")
    qpoffdiagjacobian = 0.; //TODO

  return qpoffdiagjacobian;
}

void ComponentFlux::computeOffDiagJacobian(unsigned int jvar)
{
   upwind(false, true, jvar);
}

void ComponentFlux::upwind(bool compute_res, bool compute_jac, unsigned int jvar)
{
  if (compute_jac && !_fluid_state.isFluidStateVariable(jvar))
    return;

  // Determine the variable type to take the derivative with respect to
  std::string jvar_type = _fluid_state.variableTypes(jvar);

  // We require the mobility calculated at the nodes of the element. Additionally,
  // in order to call the derivatives for the Jacobian from the FluidState UserObject,
  // determine the node id's of the nodes in this element

  unsigned int num_nodes = _test.size();
  std::vector<Real> mobility;
  mobility.resize(num_nodes);

  std::vector<unsigned int> elem_node_ids;
  elem_node_ids.resize(num_nodes);

  for (unsigned int n = 0; n < num_nodes; ++n)
  {
    elem_node_ids[n] = _current_elem->get_node(n)->id();
//    _console << "node id " << elem_node_ids[n] << " phase index " << _phase_index <<std::endl;
    mobility[n] = _component_mass_fraction[n] * _fluid_state.getNodalProperty("mobility", elem_node_ids[n], _phase_index);
  }

  // Compute the residual and jacobian without the mobility terms. Even if we are computing the jacobian
  // we still need this in order to see which nodes are upwind and which are downwind.
  DenseVector<Number> & re = _assembly.residualBlock(_var.number());
  _local_re.resize(re.size());
  _local_re.zero();

  DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), jvar);

  // Form the _local_re contribution to the residual without the mobility term, but
  // with the density (used to zero contribution where saturation is zero)
 for (_i = 0; _i < _test.size(); _i++)
   for (_qp = 0; _qp < _qrule->n_points(); _qp++)
     _local_re(_i) += _JxW[_qp] * _coord[_qp] * computeQpResidual();

 if (compute_jac)
 {
    _local_ke.resize(ke.m(), ke.n());
    _local_ke.zero();

    if (jvar == _var.number())
    {
      for (_i = 0; _i < _test.size(); _i++)
        for (_j = 0; _j < _phi.size(); _j++)
          for (_qp = 0; _qp < _qrule->n_points(); _qp++)
            _local_ke(_i, _j) += _JxW[_qp] * _coord[_qp] * computeQpJacobian();
    }
    else
    {
      for (_i = 0; _i < _test.size(); _i++)
        for (_j = 0; _j < _phi.size(); _j++)
          for (_qp = 0; _qp < _qrule->n_points(); _qp++)
            _local_ke(_i, _j) += _JxW[_qp] * _coord[_qp] * computeQpOffDiagJacobian(jvar);
    }
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
  for (unsigned int n = 0; n < num_nodes ; ++n)
  {
    if (_local_re(n) >= 1E-20)
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

    for (unsigned int n = 0; n < num_nodes ; ++n)
    {
      dtotal_mass_out[n] = 0;
      dtotal_in[n] = 0;
    }
  }

  // Perform the upwinding
    for (unsigned int n = 0; n < num_nodes ; ++n)
    {
      if (_local_re(n) >= 0 || reached_steady) // upstream node
      {
        if (compute_jac)
        {
          Real dmobility = 0.;

          if (jvar_type == "pressure")
            dmobility = _component_mass_fraction[n] *_fluid_state.getNodalProperty("dmobility_dp", elem_node_ids[n], _phase_index);

          if (jvar_type == "saturation")
            dmobility = _component_mass_fraction[n] * _fluid_state.getNodalProperty("dmobility_ds", elem_node_ids[n], _phase_index);

          for (_j = 0; _j < _phi.size(); _j++)
            _local_ke(n, _j) *= mobility[n];
            _local_ke(n, n) += dmobility * _local_re(n);
          for (_j = 0; _j < _phi.size(); _j++)
            dtotal_mass_out[_j] += _local_ke(n, _j);
        }
        _local_re(n) *= mobility[n];
        total_mass_out += _local_re(n);
      }
      else
      {
        total_in -= _local_re(n); // note the -= means the result is positive
        if (compute_jac)
          for (_j = 0; _j < _phi.size(); _j++)
            dtotal_in[_j] -= _local_ke(n, _j);
      }
    }

  // Conserve mass over all phases by proportioning the total_mass_out mass to the inflow nodes, weighted by their _local_re values
  if (!reached_steady)
  {
    for (unsigned int n = 0; n < num_nodes; ++n)
    {
      if (_local_re(n) < 0)
      {
        if (compute_jac)
          for (_j = 0; _j < _phi.size(); _j++)
          {
            _local_ke(n, _j) *= total_mass_out / total_in;
           _local_ke(n, _j) += _local_re(n) * (dtotal_mass_out[_j] / total_in - dtotal_in[_j] * total_mass_out / total_in / total_in);
          }
        _local_re(n) *= total_mass_out / total_in;
      }
    }
  }

  // Output mass flux at each node
//  _console << "Variable " << _variable << std::endl;
//  _console << "density " << _fluid_density[0] << std::endl;
/*
  for (unsigned int n = 0; n < num_nodes; ++n)
  {
   _console << "node " << n << ", mass flow " << _local_re(n) << std::endl;
  }
*/
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

  if (compute_jac)
  {
    ke += _local_ke;

    if (_has_diag_save_in && jvar == _var.number())
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
