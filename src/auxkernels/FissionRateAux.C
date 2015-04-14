/*************************************************/
/*           DO NOT MODIFY THIS HEADER           */
/*                                               */
/*                     BISON                     */
/*                                               */
/*    (c) 2014 Battelle Energy Alliance, LLC     */
/*            ALL RIGHTS RESERVED                */
/*                                               */
/*   Prepared by Battelle Energy Alliance, LLC   */
/*     Under Contract No. DE-AC07-05ID14517      */
/*     With the U. S. Department of Energy       */
/*                                               */
/*     See COPYRIGHT for full restrictions       */
/*************************************************/

#include "FissionRateAux.h"

#include "Function.h"

template<>
InputParameters validParams<FissionRateAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addParam<Real>("value",1.0,"If no function is provided, the value of the fission rate, otherwise, a scaling factor for the function.");
  params.addParam<FunctionName>("function", "", "The function that describes the fission rate.");
  return params;
}

FissionRateAux::FissionRateAux(const std::string & name, InputParameters parameters)
  :AuxKernel(name, parameters),
   _value(getParam<Real>("value")),
   _has_function(getParam<FunctionName>("function") != ""),
   _function( _has_function ? &getFunction("function") : NULL )
{
  if (_has_function && !_function)
  {
    Moose::out << "Unable to find function in fission rate aux." << std::endl;
    libmesh_error();
  }
}

Real
FissionRateAux::computeValue()
{
  Real value( _value );
  if ( _has_function )
  {
    if ( isNodal() )
    {
      mooseAssert( _current_node, "ERROR:  FissionRateAux reports nodal but no node defined." );
      const Node & node = *_current_node;
      value *= _function->value(_t, node);
    }
    else
      value *= _function->value(_t, _q_point[_qp]);
  }
  if (value < 0)
  {
    mooseError("Negative fission rate in FissionRateAux");
  }
  return value;
}
