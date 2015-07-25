/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#include "MultiphaseProblem.h"

template<>
InputParameters validParams<MultiphaseProblem>()
{
  InputParameters params = validParams<FEProblem>();
  params.addRequiredParam<NonlinearVariableName>("saturation_variable", "The saturation non-linear variable.");
  return params;
}


MultiphaseProblem::MultiphaseProblem(const InputParameters & params) :
    FEProblem(params),
    // In the following have to get the names of the variables, and then find their numbers in initialSetup,
    // as their numbers won't be defined at the moment of instantiation of this class
    _bounded_var_name(params.get<NonlinearVariableName>("saturation_variable")),
    _bounded_var_num(0)
{}

MultiphaseProblem::~MultiphaseProblem()
{}

void
MultiphaseProblem::initialSetup()
{
  // the first argument to getVariable is threadID - i hope the following always works
  unsigned int tid = 0;
  MooseVariable & bounded = getVariable(tid, _bounded_var_name);

  // some checks
  if (!bounded.isNodal())
    mooseError("The bounded variables must be nodal variables in MultiphaseProblem");

  // extract the required info
  _bounded_var_num = bounded.number();

  FEProblem::initialSetup();
}


bool
MultiphaseProblem::shouldUpdateSolution()
{
  return true;
}

bool
MultiphaseProblem::updateSolution(NumericVector<Number>& vec_solution, NumericVector<Number>& ghosted_solution)
{
  bool updatedSolution = false;  // this gets set to true if we needed to enforce the bound at any node

  unsigned int sys_num = getNonlinearSystem().number();

  // For parallel procs i believe that i have to use local_nodes_begin, rather than just nodes_begin
  // _mesh comes from SystemBase (_mesh = getNonlinearSystem().subproblem().mesh(), and subproblem is this object)
  MeshBase::node_iterator nit = _mesh.getMesh().local_nodes_begin();
  const MeshBase::node_iterator nend = _mesh.getMesh().local_nodes_end();

  for ( ; nit != nend; ++nit)
  {
    const Node & node = *(*nit);

    // dofs[0] is the dof number of the bounded variable at this node
    std::vector<dof_id_type> dofs(1);
    dofs[0] = node.dof_number(sys_num, _bounded_var_num, 0);

    // soln[0] is the value of the bounded variable at this node
    std::vector<Number> soln(1);
    vec_solution.get(dofs, soln);

    // do the bounding
    if (soln[0] < 0.0)
    {
      vec_solution.set(dofs[0], 0.0); // set the bounded variable equal to 0 if it is less than 0.
      updatedSolution = true;
    }
    if (soln[0] > 1.0)
    {
      vec_solution.set(dofs[0], 1.0); // set the bounded variable equal to 1 if it is greater than 1.
      updatedSolution = true;
    }

  }

  // The above vec_solution.set calls potentially added "set" commands to a queue
  // The following actions the queue (doing MPI commands if necessary), so
  // vec_solution will actually be modified by this following command
  vec_solution.close();

  // if any proc updated the solution, all procs will know about it
  _communicator.max(updatedSolution);

  if (updatedSolution)
  {
    ghosted_solution = vec_solution;
    ghosted_solution.close();
  }

  return updatedSolution;

}

