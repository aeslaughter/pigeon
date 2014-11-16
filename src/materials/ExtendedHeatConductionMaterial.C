#include "ExtendedHeatConductionMaterial.h"

template<>
InputParameters validParams<ExtendedHeatConductionMaterial>()
{
  InputParameters params = validParams<HeatConductionMaterial>();
  
  params.addParam<Real>("density","The density value");
  params.addParam<FunctionName>("density_function","","Density as a function of temperature");

  return params;
}

ExtendedHeatConductionMaterial::ExtendedHeatConductionMaterial(const std::string & name, InputParameters parameters) :
    HeatConductionMaterial(name, parameters),
// check if density was inputed, if not don't try to get it
_my_density(isParamValid("density") ? getParam<Real>("density") : 0),
// declare density property for use in kernels
_density(declareProperty<Real>("density")),
_density_function(getParam<FunctionName>("density_function") != "" ? &getFunction("density_function") : NULL)
{
  if (_density_function && !_has_temp)
  {
    mooseError("Must couple with temperature if using density function");
  }
  if (isParamValid("density") && _density_function)
  {
    mooseError("Cannot define both density and density temperature function");
  }
}

void
ExtendedHeatConductionMaterial::computeProperties()
{
  HeatConductionMaterial::computeProperties();
  
  for (unsigned int qp(0); qp < _qrule->n_points(); ++qp)
  {
    if (_density_function)
    {
      Point p;
      // using function evaluation to use _temperature instead of the typical time. p passed in because it's expected
      _density[qp] = _density_function->value(_temperature[qp], p);
    }
    else
    {
      _density[qp] = _my_density;
    }
  }
}
