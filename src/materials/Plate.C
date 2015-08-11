/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "Plate.h"

template<>
InputParameters validParams<Plate>()
{

  InputParameters params = validParams<Material>();

  // Parameters
  params.addParam<Real>("l",1,"Plate thickness (for 2D simplifications) (m).");
  params.addParam<Real>("rho",1,"Density (kg/m^3).");
  params.addParam<Real>("alpha",1,"Thermal diffusivity (m^2/s).");
  params.addParam<Real>("c_p",1,"Specific heat capacity (J/(kg*K).");

  return params;
}

Plate::Plate(const InputParameters & parameters) :
    Material(parameters),
    _l(getParam<Real>("l")),
    _thickness(declareProperty<Real>("thickness")),
    _rho(getParam<Real>("rho")),
    _density(declareProperty<Real>("density")),
    _alpha(getParam<Real>("alpha")),
    _diffusivity(declareProperty<Real>("diffusivity")),
    _c_p(getParam<Real>("c_p")),
    _specific_heat(declareProperty<Real>("specific_heat"))
{}

void
Plate::computeQpProperties()
{
  
  _thickness[_qp] = _l;
  _density[_qp] = _rho;
  _diffusivity[_qp] = _alpha;
  _specific_heat[_qp] = _c_p;

}
