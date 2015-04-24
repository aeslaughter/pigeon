/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate pressure of the gas phase      */
/* for multiphase flow in porous media using the capillary      */
/* pressure defined in the Capillary Pressure UserObject.       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "GasPressureAux.h"

template<>
InputParameters validParams<GasPressureAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredCoupledVar("liquid_pressure_variable", "The pressure variable corresponding to the liquid phase.");
  params.addRequiredCoupledVar("capillary_pressure_variable", "The capillary pressure auxillary variable.");
  return params;
}

GasPressureAux::GasPressureAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _liquid_pressure(coupledValue("liquid_pressure_variable")),
    _capillary_pressure(coupledValue("capillary_pressure_variable"))
{}

Real GasPressureAux::computeValue()
{
    return _liquid_pressure[_qp] + _capillary_pressure[_qp]; // The gas pressure
}
