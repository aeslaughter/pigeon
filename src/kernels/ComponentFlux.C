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
  params.addRequiredCoupledVar("fluid_density_variables", "The list of fluid density auxiallary variables for each phase.");
  params.addRequiredCoupledVar("fluid_viscosity_variables", "The list of fluid viscosity auxillary variables for each phase.");
  params.addRequiredCoupledVar("component_mass_fraction_variables", "The list of mass fraction of the kernels component in each phase.");
  params.addRequiredCoupledVar("relperm_variables", "The list of relative permeability auxillary variables for each phase.");
  MooseEnum primary_variable_type("pressure saturation mass_fraction");
  params.addRequiredParam<MooseEnum>("primary_variable_type", primary_variable_type, "The type of primary variable for this kernel (e.g. pressure, saturation or component mass fraction");
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  params.addParam<unsigned int>("phase_index", 0, "The index of the primary variable this kernel acts on");
  return params;
}

ComponentFlux::ComponentFlux(const std::string & name,
                       InputParameters parameters) :
  Kernel(name, parameters),
  _permeability(getMaterialProperty<RealTensorValue>("permeability")),
  _gravity(getMaterialProperty<RealVectorValue>("gravity")),
  _phase_flux_no_mobility(getMaterialProperty<std::vector<RealVectorValue> >("phase_flux_no_mobility")),
  _primary_variable_type(getParam<MooseEnum>("primary_variable_type")),
  _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
  _phase_index(getParam<unsigned int>("phase_index"))

{
  // The number of phases in the given fluid state model
  _num_phases = _fluid_state.numPhases();

  // Check that the required number of auxillary variables have been provided
  if (coupledComponents("fluid_density_variables") != _num_phases)
    mooseError("The number of phase densities provided in the ComponentFlux kernel is not equal to the number of phases in the FluidState UserObject");
  if (coupledComponents("fluid_viscosity_variables") != _num_phases)
    mooseError("The number of phase viscosities provided in the ComponentFlux kernel is not equal to the number of phases in the FluidState UserObject");
  if (coupledComponents("relperm_variables") != _num_phases)
    mooseError("The number of phase relative permeabilities provided in the ComponentFlux kernel is not equal to the number of phases in the FluidState UserObject");
  if (coupledComponents("component_mass_fraction_variables") != _num_phases)
    mooseError("The number of phase components provided in the ComponentFlux kernel is not equal to the number of phases in the FluidState UserObject");

  // Filling the vectors with VariableValue pointers
  _fluid_density.resize(_num_phases);
  _fluid_viscosity.resize(_num_phases);
  _component_mass_fraction.resize(_num_phases);
  _fluid_relperm.resize(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    _fluid_density[n] = &coupledNodalValue("fluid_density_variables", n);
    _fluid_viscosity[n] = &coupledNodalValue("fluid_viscosity_variables", n);
    _component_mass_fraction[n] = &coupledNodalValue("component_mass_fraction_variables", n);
    _fluid_relperm[n] = &coupledNodalValue("relperm_variables", n);
  }
}

Real ComponentFlux::computeQpResidual()
{
  return 0.; // Not used in the upwinding code.
}

void ComponentFlux::computeResidual()
{
  upwind(true, false, 0.);
}

Real ComponentFlux::computeQpJacobian()
{
  return 0.0; // Not used in the upwinding code.
/*
  Real jacobian = 0.0;

  if (_primary_variable_type == "saturation") {

    jacobian = 0.0;
  }

  if (_primary_variable_type == "pressure") {

    jacobian = _grad_test[_i][_qp] * (_permeability[_qp] * (*_fluid_density[0])[_qp]* _grad_phi[_j][_qp] / (*_fluid_viscosity[0])[_qp]);

  }

  if (_primary_variable_type == "mass_fraction") {

    jacobian = 0.;
  }

  return jacobian;
  */
}

void ComponentFlux::computeJacobian()
{
   upwind(false, true, _var.number());
}

Real ComponentFlux::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.;
}

void ComponentFlux::upwind(bool compute_res, bool compute_jac, unsigned int jvar)
{
  std::vector<std::vector<Real> > mobility;
  Real mobtmp;

  unsigned int num_nodes = _test.size();
  mobility.resize(_num_phases);

  // The mobility calculated at the nodes for each phase. Note that the component mass fraction is not
  // included yet. Instead, determine the flux of gas and liquid, and then scale by the mass fraction.
  // Also, fluid density is used in the local_re calculations later.
  for (unsigned int n = 0; n < _num_phases; ++n)
    for (_i = 0; _i < num_nodes; _i++)
    {
      mobtmp = (*_fluid_relperm[n])[_i] * (*_fluid_density[n])[_i] / (*_fluid_viscosity[n])[_i];
      mobility[n].push_back(mobtmp);
    }

  // Compute the residual and jacobian without the mobility terms. Even if we are computing the jacobian
  // we still need this in order to see which nodes are upwind and which are downwind.
  DenseVector<Number> & re = _assembly.residualBlock(_var.number());
  _local_re.resize(re.size());

  DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), jvar);
  _local_ke.resize(ke.m(), ke.n());


  // Vector of _local_re's and _local_ke's for each phase.
  std::vector<DenseVector<Number> > local_re_phase;
  local_re_phase.resize(_num_phases);

  std::vector<DenseMatrix<Number> > local_ke_phase;
  local_ke_phase.resize(_num_phases);

  Real qpresidual;
  Real qpjacobian;

  // Loop over each phsae and form the _local_re contribution to the residual without the mobility term, but
  // with the density (used to zero contribution where saturation is zero)
  // Note that computeQpResidual is not called, instead the residual is calculated at each qp
  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    // Set the local_re = 0 initially for each phase
    _local_re.zero();
    for (_i = 0; _i < _test.size(); _i++)
      for (_qp = 0; _qp < _qrule->n_points(); _qp++)
      {
        qpresidual = _grad_test[_i][_qp] * (_permeability[_qp] * (*_fluid_density[n])[_qp] *
          _phase_flux_no_mobility[_qp][n]);
        _local_re(_i) += _JxW[_qp] * _coord[_qp] * qpresidual;
      }
    local_re_phase[n] = _local_re;

    if (compute_jac)
    {
      _local_ke.zero();

      for (_i = 0; _i < _test.size(); _i++)
        for (_j = 0; _j < _phi.size(); _j++)
          for (_qp = 0; _qp < _qrule->n_points(); _qp++)
          {
            if (_primary_variable_type == "pressure") //TODO: other primary variable types
              qpjacobian = _grad_test[_i][_qp] * (_permeability[_qp] * (*_fluid_density[n])[_qp] *_grad_phi[_j][_qp]);
            _local_ke(_i, _j) += _JxW[_qp] * _coord[_qp] * qpjacobian;
          }
      local_ke_phase[n] = _local_ke;
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


  for (unsigned int n = 0; n < _num_phases; ++n)
  {

  // FIRST:
  // this is a dirty way of getting around precision loss problems
  // and problems at steadystate where upwinding oscillates from
  // node-to-node causing nonconvergence.
  bool reached_steady = true;
  for (unsigned int nodenum = 0; nodenum < num_nodes ; ++nodenum)
  {
    if (local_re_phase[n](nodenum) >= 1E-20)
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


  // Perform the upwinding over all phases
    for (unsigned int nodenum = 0; nodenum < num_nodes ; ++nodenum)
    {
      if (local_re_phase[n](nodenum) >= 0 || reached_steady) // upstream node
      {
        if (compute_jac)
        {
          for (_j = 0; _j < _phi.size(); _j++)
            local_ke_phase[n](nodenum, _j) *= mobility[n][nodenum];
// FIX THIS
//        _local_ke(nodenum, nodenum) += _dmobility_dv[nodenum][dvar]*_local_re(nodenum);
          for (_j = 0; _j < _phi.size(); _j++)
            dtotal_mass_out[_j] += local_ke_phase[n](nodenum, _j);
        }
        local_re_phase[n](nodenum) *= mobility[n][nodenum];
        total_mass_out += local_re_phase[n](nodenum);
      }
      else
      {
        total_in -= local_re_phase[n](nodenum); // note the -= means the result is positive
        if (compute_jac)
          for (_j = 0; _j < _phi.size(); _j++)
            dtotal_in[_j] -= local_ke_phase[n](nodenum, _j);
      }
    }

  // Conserve mass over all phases by proportioning the total_mass_out mass to the inflow nodes, weighted by their _local_re values
  if (!reached_steady)
  {
    for (unsigned int nodenum = 0; nodenum < num_nodes; ++nodenum)
    {
      if (local_re_phase[n](nodenum) < 0)
      {
        if (compute_jac)
          for (_j = 0; _j < _phi.size(); _j++)
          {
            local_ke_phase[n](nodenum, _j) *= total_mass_out/total_in;
            local_ke_phase[n](nodenum, _j) += local_re_phase[n](nodenum)*(dtotal_mass_out[_j]/total_in - dtotal_in[_j]*total_mass_out/total_in/total_in);
          }
        local_re_phase[n](nodenum) *= total_mass_out/total_in;
      }
    }
  }
}

  // Sum all of the contributions to the residual and jacobian of each phase,
  // scaled by the component mass fraction.
  DenseVector<Number> local_re_sum;
  local_re_sum.resize(re.size());
  local_re_sum.zero();

  DenseMatrix<Number> local_ke_sum;
  local_ke_sum.resize(ke.m(), ke.n());
  local_ke_sum.zero();

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    for (unsigned int nodenum = 0; nodenum < num_nodes; ++nodenum)
    {
      local_re_sum(nodenum) += local_re_phase[n](nodenum) * (*_component_mass_fraction[n])[nodenum];
      if (compute_jac)
        for (_j = 0; _j < _phi.size(); _j++)
          local_ke_sum(nodenum, _j) += local_ke_phase[n](nodenum, _j) * (*_component_mass_fraction[n])[nodenum];
    }
  }

  // Add results to the Residual or Jacobian
  if (compute_res)
  {
      re += local_re_sum;

    if (_has_save_in)
    {
      Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
      for (unsigned int i = 0; i < _save_in.size(); i++)
        _save_in[i]->sys().solution().add_vector(local_re_sum, _save_in[i]->dofIndices());
    }
  }

  if (compute_jac)
  {
    ke += local_ke_sum;

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
