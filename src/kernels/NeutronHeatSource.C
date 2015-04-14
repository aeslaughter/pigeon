#include "NeutronHeatSource.h"

#include "BurnupFunction.h"

template<>
InputParameters validParams<NeutronHeatSource>()
{
  InputParameters params = validParams<Kernel>();

  params.addCoupledVar("fission_rate", "Coupled Fission Rate");

  params.addCoupledVar("max_fission_rate", "Coupled Max Fission Rate");
  params.addParam<PostprocessorName>("decay_heat_function", "Postprocessor giving the decay heat curve");

  params.addParam<FunctionName>("burnup_function", "Burnup function");

  params.addParam<Real>("energy_per_fission", 3.28451e-11, "Energy Released per Fission");
  params.addParam<std::string>("units", "J/fission", "Energy Released per Fission");

  params.addParam<FunctionName>("rod_ave_lin_pow", "Function describing rod average linear power");
  params.addParam<FunctionName>("axial_profile", "Function describing the axial profile of power");
  params.addParam<Real>("outer_diameter", "Outer diameter");
  params.addParam<Real>("inner_diameter", 0, "Inner diameter");
  params.addParam<Real>("area", "Cross sectional area");
  params.addParam<Real>("fraction", 1, "Fraction of power applied");

  return params;
}

NeutronHeatSource::NeutronHeatSource(const std::string & name, InputParameters parameters)
  :Kernel(name, parameters),
   _energy_per_fission(getParam<Real>("energy_per_fission")),
   _units(getParam<std::string>("units")),
   _has_fission_rate(isCoupled("fission_rate")),
   _fission_rate(_has_fission_rate ? coupledValue("fission_rate") : _zero),

   _has_max_fission_rate(isCoupled("max_fission_rate")),
   _max_fission_rate(_has_max_fission_rate ? coupledValue("max_fission_rate") : _zero),
   _decay_heat_function( isParamValid("decay_heat_function") ? getPostprocessorValue("decay_heat_function") : _real_zero),

   _burnup_function( isParamValid("burnup_function") ?
                     dynamic_cast<BurnupFunction*>(&getFunction("burnup_function")) : NULL ),

   _rod_ave_lin_pow( isParamValid("rod_ave_lin_pow") ?
                     &getFunction("rod_ave_lin_pow") : NULL ),
   _axial_profile( isParamValid("axial_profile") ?
                   &getFunction("axial_profile") : NULL ),
   _has_diameter(isParamValid("outer_diameter")),
   _outer_diameter(_has_diameter ?
                   parameters.get<Real>("outer_diameter") : -1),
   _inner_diameter(parameters.get<Real>("inner_diameter")),
   _has_area(isParamValid("area")),
   _area(_has_area ? parameters.get<Real>("area") : 1),
   _fraction(parameters.get<Real>("fraction"))

{
  if (!_has_fission_rate && !_burnup_function && !_rod_ave_lin_pow)
  {
    mooseError("Must specify fission_rate, burnup_function, or rod_ave_lin_pow NeutronHeatSource");
  }
  if ((int(_has_fission_rate) + int(_burnup_function!=NULL) + int(_rod_ave_lin_pow!=NULL)) > 1)
  {
    mooseError("Cannot specify more than one of fission_rate, burnup_function, and rod_ave_lin_pow in NeutronHeatSource");
  }
  if ((_has_fission_rate || _burnup_function) && _axial_profile)
  {
    mooseError("Cannot specify fission_rate or burnup_function with axial_profile in NeutronHeatSource");
  }
  if (_rod_ave_lin_pow && !(_has_diameter || _has_area))
  {
    mooseError("Must specify either diameter or area with rod_ave_lin_pow");
  }
  if (_has_diameter && _has_area)
  {
    mooseError("Cannot specify diameter and area in NeutronHeatSource");
  }
  if ((_has_fission_rate || _burnup_function) && (_has_diameter || _has_area))
  {
    mooseError("Cannot specify fission_rate or burnup_function with either diameter or area in NeutronHeatSource");
  }
  if (_has_diameter && _outer_diameter < _inner_diameter)
  {
    mooseError("outer_diameter must be larger than inner_diameter in NeutronHeatSource");
  }
  if (_has_diameter && (_outer_diameter < 0 || _inner_diameter < 0))
  {
    mooseError("Diameter must be greater than zero in NeutronHeatSource");
  }
}

Real
NeutronHeatSource::computeQpResidual()
{
  Real value(0);
  if (_has_fission_rate || _burnup_function)
  {
    Real fission_rate = _has_fission_rate ? _fission_rate[_qp] : _burnup_function->fissionRate(_q_point[_qp]);
    value = fission_rate * _energy_per_fission;

    if (_decay_heat_function > 0)
    {
      value =  _max_fission_rate[_qp] * _energy_per_fission * _decay_heat_function;
    }
  }
  else
  {
    value = 1/_area;
    if (_has_diameter)
    {
      value = 1 / (0.25 * M_PI * (_outer_diameter*_outer_diameter - _inner_diameter*_inner_diameter));
    }
    mooseAssert(_rod_ave_lin_pow, "Error: rod_ave_lin_pow function not found");
    value *= _rod_ave_lin_pow->value(_t, _q_point[_qp]);
    if (_axial_profile)
    {
      value *= _axial_profile->value(_t, _q_point[_qp]);
    }
  }
  return -_test[_i][_qp] * value * _fraction;
}
