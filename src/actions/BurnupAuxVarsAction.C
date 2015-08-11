#include "BurnupAuxVarsAction.h"

#include "Factory.h"
#include "FEProblem.h"
#include "Parser.h"
#include "libmesh/string_to_enum.h"
#include "MooseApp.h"

template<>
InputParameters validParams<BurnupAuxVarsAction>()
{
  InputParameters params = validParams<Action>();

  params.addParam<std::string>("order", "CONSTANT",  "Specifies the order of the FE shape function to use for this variable");
  params.addParam<std::string>("family", "MONOMIAL", "Specifies the family of FE shape functions to use for this variable");
  params.addRequiredParam<std::vector<SubdomainName> >("block", "The block id where this variable lives");
  params.addParam<std::string>("N235", "", "Specifies that the concentration of 235 is required.");
  params.addParam<std::string>("N238", "", "Specifies that the concentration of 238 is required.");
  params.addParam<std::string>("N239", "", "Specifies that the concentration of 239 is required.");
  params.addParam<std::string>("N240", "", "Specifies that the concentration of 240 is required.");
  params.addParam<std::string>("N241", "", "Specifies that the concentration of 241 is required.");
  params.addParam<std::string>("N242", "", "Specifies that the concentration of 242 is required.");
  params.addParam<std::string>("RPF", "", "Specifies that the radial power factor is required.");

  return params;
}

BurnupAuxVarsAction::BurnupAuxVarsAction(InputParameters params) :
  Action(params),
  _order(getParam<std::string>("order")),
  _family(getParam<std::string>("family")),
  _N235(getParam<std::string>("N235")),
  _N238(getParam<std::string>("N238")),
  _N239(getParam<std::string>("N239")),
  _N240(getParam<std::string>("N240")),
  _N241(getParam<std::string>("N241")),
  _N242(getParam<std::string>("N242")),
  _RPF(getParam<std::string>("RPF"))
{
  mooseAssert(!getParam<std::vector<SubdomainName> >("block").empty(), "Blocks must be specified in BurnupAuxVarsAction");
}

void
BurnupAuxVarsAction::act()
{
  std::set<SubdomainID> blocks;

  std::vector<SubdomainName> block_param = getParam<std::vector<SubdomainName> >("block");
  for (std::vector<SubdomainName>::iterator it = block_param.begin(); it != block_param.end(); ++it)
    blocks.insert(_problem->mesh().getSubdomainID(*it));

  std::vector<std::string> var_value;
  var_value.push_back("burnup");
  var_value.push_back("fission_rate");
  if (_N235 != "")
  {
    var_value.push_back(_N235);
  }
  if (_N238 != "")
  {
    var_value.push_back(_N238);
  }
  if (_N239 != "")
  {
    var_value.push_back(_N239);
  }
  if (_N240 != "")
  {
    var_value.push_back(_N240);
  }
  if (_N241 != "")
  {
    var_value.push_back(_N241);
  }
  if (_N242 != "")
  {
    var_value.push_back(_N242);
  }
  if (_RPF != "")
  {
    var_value.push_back(_RPF);
  }

  for (unsigned i = 0; i < var_value.size(); ++i)
  {

    _problem->addAuxVariable(var_value[i],
                             FEType(Utility::string_to_enum<Order>(_order),
                                    Utility::string_to_enum<FEFamily>(_family)),
                             &blocks);
  }

}
