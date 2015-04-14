#include "BurnupFunctionAction.h"

#include "BurnupFunction.h"

template<>
InputParameters validParams<BurnupFunctionAction>()
{
  InputParameters params = validParams<AddFunctionAction>();
  // Populate the required type parameter
  params.suppressParameter<std::string>("type");
  params.set<std::string>("type") = "BurnupFunction";
  // Restrict this object to be only allowed to build BurnupFunction
  params.registerBuildableTypes("BurnupFunction");
  // Hide the MooseObject params in syntax dumps
  params.mooseObjectSyntaxVisibility(false);
  params.collapseSyntaxNesting(true);
  // Add BurnupFunction parameters
  params += validParams<BurnupFunction>();

  return params;
}

BurnupFunctionAction::BurnupFunctionAction(const std::string & name,
                                           InputParameters params) :
  AddFunctionAction(name, params)
{
}
