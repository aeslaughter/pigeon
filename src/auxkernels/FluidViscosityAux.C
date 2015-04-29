/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate fluid viscosity using the given*/
/* Fluid Property UserObject for multiphase flow in porous      */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidViscosityAux.h"

template<>
InputParameters validParams<FluidViscosityAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("fluid_property_uo", "Name of the User Object defining the fluid properties");
  params.addRequiredCoupledVar("pressure_variable", "The pressure variable corresponding to the fluid phase.");
  params.addRequiredCoupledVar("temperature_variable", "The temperature variable corresponding to the fluid phase.");
  return params;
}

FluidViscosityAux::FluidViscosityAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_property(getUserObject<FluidProperties>("fluid_property_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable"))
{}

Real FluidViscosityAux::computeValue()
{
  Real density = _fluid_property.fluidDensity(_pressure[_qp], _temperature[_qp]);

  return _fluid_property.fluidViscosity(_temperature[_qp], density);
}
