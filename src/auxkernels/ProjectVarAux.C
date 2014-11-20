#include "ProjectVarAux.h"

template<>
InputParameters validParams<ProjectVarAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addParam<Real>("value", 0.0, "Scalar value used for our auxiliary calculation");
  params.addRequiredCoupledVar("coupled", "Coupled variable");
  return params;
}

ProjectVarAux::ProjectVarAux(const std::string & name, InputParameters parameters) :
    AuxKernel(name, parameters),

    // We can couple in a value from one of our kernels with a call to coupledValueAux
    _coupled_val(coupledValue("coupled")),

    // Set our member scalar value from InputParameters (read from the input file)
    _value(getParam<Real>("value"))
{}

/**
 * Auxiliary Kernels override computeValue() instead of computeQpResidual().  Aux Variables
 * are calculated either one per elemenet or one per node depending on whether we declare
 * them as "Elemental (Constant Monomial)" or "Nodal (First Lagrange)".  No changes to the
 * source are necessary to switch from one type or the other.
 */
Real
ProjectVarAux::computeValue()
{
  return _coupled_val[_qp] + _value;
}
