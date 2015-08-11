//This aux kernel is used to "calculate" fast neutron flux by scaling the fission rate
//input by the user.  This is not the way to calculate fast neutron flux, but it is used
//here as a place-holder until a proper calculation for fast neutron flux at the clad is implemented.
#include "FastNeutronFluxAux.h"
#include "Material.h"

template<>
InputParameters validParams<FastNeutronFluxAux>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addParam<Real>("factor", "The fast neutron flux if the function, RALP, and q_variable are not given. A scaling factor if the function, RALP, or q_variable is given.  If RALP or q_variable is given, it is recommended to use a value of 3e13 (n/(m^2s)/(W/m)).");
  params.addParam<FunctionName>("function", "The function that describes the fast neutron flux");
  params.addParam<FunctionName>("rod_ave_lin_pow", "The power history function");
  params.addParam<FunctionName>("axial_power_profile", "The axial power profile function");
  params.addCoupledVar("q_variable", "Variable holding linear heat rate in pellet in W/m");

  return params;
}

FastNeutronFluxAux::FastNeutronFluxAux(const InputParameters & parameters) :
  AuxKernel(parameters),
  _factor(isParamValid("factor") ?
          getParam<Real>("factor") : 1),
  _function(isParamValid("function") ?
            &getFunction("function") :
            NULL),
  _ralp(isParamValid("rod_ave_lin_pow") ?
        &getFunction("rod_ave_lin_pow") :
        NULL),
  _axial_profile(isParamValid("axial_power_profile") ?
                 &getFunction("axial_power_profile") :
                 NULL),
  _q_variable(isParamValid("q_variable") ?
              &coupledValue("q_variable") :
              NULL)
{
  bool has_factor(isParamValid("factor"));
  if (!has_factor && !_function && !_ralp && !_q_variable)
  {
    mooseError("FastNeutronFluxAux requires at least one of factor, function, rod_ave_lin_pow, or q_variable");
  }
  if ((_function && _ralp) || (_function && _q_variable) || (_ralp && _q_variable))
  {
    mooseError("FastNeutronFluxAux: must specify only one of function, rod_ave_lin_pow, and q_variable");
  }
  if (!has_factor && (_ralp || _q_variable))
  {
    mooseError("FastNeutronFluxAux: must specify factor when specifying rod_ave_lin_pow");
  }
  if (_ralp && !_axial_profile)
  {
    mooseWarning("FastNeutronFluxAux: rod_ave_lin_pow given but axial_power_profile was not");
  }
}

Real
FastNeutronFluxAux::computeValue()
{
  Point point;
  if ( isNodal() )
  {
    point = *_current_node;
  }
  else
  {
    point = _q_point[_qp];
  }
  Real fnf(_factor);
  if ( _ralp )
  {
    fnf *= _ralp->value( _t, point );
  }
  else if ( _function )
  {
    fnf *= _function->value( _t, point );
  }
  else if ( _q_variable )
  {
    fnf *= (*_q_variable)[_qp];
  }
  if ( _axial_profile )
  {
    fnf *= _axial_profile->value( _t, point );
  }
  return fnf;
}
