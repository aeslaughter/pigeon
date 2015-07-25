/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef MULTIPHASEPROBLEM_H
#define MULTIPHASEPROBLEM_H

#include "FEProblem.h"

class MultiphaseProblem;

template<>
InputParameters validParams<MultiphaseProblem>();

 /**
  * Constrain the saturation solution to be 0 <= sat <= 1.0 during
  * the nonlinear iteration process.
  */

class MultiphaseProblem : public FEProblem
{
public:
  MultiphaseProblem(const InputParameters & params);
  virtual ~MultiphaseProblem();

  /**
   * extracts the moose variable numbers associated with the saturation variable
   */
  virtual void initialSetup();

  /// returns true, indicating that updateSolution should be run
  virtual bool shouldUpdateSolution();

  /**
   * Does the bounding by modifying vec_solution, and then ghosted_solution
   * @param vec_solution is the solution that Petsc says we should use.
   * @param ghosted_solution is a ghosted version of vec_solution.
   * @return true if vec_solution was changed at a node in order to respect the bounds
   */
  virtual bool updateSolution(NumericVector<Number>& vec_solution, NumericVector<Number>& ghosted_solution);


protected:

  /// name of the bounded variable
  NonlinearVariableName _bounded_var_name;

  /// internal moose variable number associated with _bounded_var
  unsigned int _bounded_var_num;

};

#endif //MULTIPHASEPROBLEM_H
