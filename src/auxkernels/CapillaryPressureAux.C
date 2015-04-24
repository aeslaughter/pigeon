/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate capillary pressure             */
/* Pc = Pg - Pw for multiphase flow in porous media using the   */
/* capillary pressure relationship defined in the Capillary     */
/* Pressure UserObject.                                         */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "CapillaryPressureAux.h"

template<>
InputParameters validParams<CapillaryPressureAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("capillary_pressure_uo", "Name of the User Object defining the capillary pressure function");
  params.addRequiredCoupledVar("liquid_saturation_variable", "The liquid saturation variable.");
  return params;
}

CapillaryPressureAux::CapillaryPressureAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _capillary_pressureUO(getUserObject<CapillaryPressure>("capillary_pressure_uo")),
    _liquid_saturation(coupledValue("liquid_saturation_variable"))
{}

Real CapillaryPressureAux::computeValue()
{
  return _capillary_pressureUO.capillaryPressure(_liquid_saturation[_qp]); 
}
