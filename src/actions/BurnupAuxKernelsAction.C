#include "BurnupAuxKernelsAction.h"
#include "Factory.h"
#include "FEProblem.h"
#include "MooseApp.h"

template<>
InputParameters validParams<BurnupAuxKernelsAction>()
{
  InputParameters params = validParams<Action>();

  params.addRequiredParam<std::vector<SubdomainName> >("block", "The blocks where radial power factor should be computed.");
  // 205 MeV/fission * 1.6022e-13 J/MeV = 3.28451e-11 J/fission
  params.addParam<Real>("energy_per_fission", 3.28451e-11, "Energy released per fission (J/fission)");
  params.addParam<std::string>("N235", "", "Specifies that the concentration of 235 is required.");
  params.addParam<std::string>("N238", "", "Specifies that the concentration of 238 is required.");
  params.addParam<std::string>("N239", "", "Specifies that the concentration of 239 is required.");
  params.addParam<std::string>("N240", "", "Specifies that the concentration of 240 is required.");
  params.addParam<std::string>("N241", "", "Specifies that the concentration of 241 is required.");
  params.addParam<std::string>("N242", "", "Specifies that the concentration of 242 is required.");
  params.addParam<std::string>("RPF", "", "Specifies that the radial power factor is required.");

  return params;
}

BurnupAuxKernelsAction::BurnupAuxKernelsAction(InputParameters params) :
  Action(params),
  _blocks(getParam<std::vector<SubdomainName> >("block")),
  _energy_per_fission(getParam<Real>("energy_per_fission")),
  _N235(getParam<std::string>("N235")),
  _N238(getParam<std::string>("N238")),
  _N239(getParam<std::string>("N239")),
  _N240(getParam<std::string>("N240")),
  _N241(getParam<std::string>("N241")),
  _N242(getParam<std::string>("N242")),
  _RPF(getParam<std::string>("RPF"))
{
}

void
BurnupAuxKernelsAction::act()
{
  std::vector<std::string> var_name;
  std::vector<VariableName> var_value;
  var_name.push_back("fission_rate");
  var_value.push_back("fission_rate");
  if (_N235 != "")
  {
    var_name.push_back("N235");
    var_value.push_back(_N235);
  }
  if (_N238 != "")
  {
    var_name.push_back("N238");
    var_value.push_back(_N238);
  }
  if (_N239 != "")
  {
    var_name.push_back("N239");
    var_value.push_back(_N239);
  }
  if (_N240 != "")
  {
    var_name.push_back("N240");
    var_value.push_back(_N240);
  }
  if (_N241 != "")
  {
    var_name.push_back("N241");
    var_value.push_back(_N241);
  }
  if (_N242 != "")
  {
    var_name.push_back("N242");
    var_value.push_back(_N242);
  }
  if (_RPF != "")
  {
    var_name.push_back("RPF");
    var_value.push_back(_RPF);
  }

  InputParameters params = _factory.getValidParams("BurnupGrid");

  // Extract global params (like density)
  _app.parser().extractParams(_name, params);

  params.set<std::vector<SubdomainName> >("block") = _blocks;
  params.set<AuxVariableName>("variable") = "burnup";

  MultiMooseEnum execute_options = SetupInterface::getExecuteOptions();
  execute_options = "timestep_begin";

  params.set<MultiMooseEnum>("execute_on") = execute_options;
  params.set<FunctionName>("burnup_function") = getShortName();
  params.set<Real>("energy_per_fission") = _energy_per_fission;
  for (unsigned i(0); i < var_name.size(); ++i)
  {
    params.set<std::vector<VariableName> >(var_name[i]) = std::vector<VariableName>(1, var_value[i]);
  }
  _problem->addAuxKernel("BurnupGrid",
                         "BurnupGrid",
                         params);

  // Now, the other aux kernels.
  for (unsigned i(0); i < var_name.size(); ++i)
  {
    InputParameters params = _factory.getValidParams("SelfAux");

    // Extract global params
    _app.parser().extractParams(_name, params);
    params.set<AuxVariableName>("variable") = var_name[i];
    params.set<MultiMooseEnum>("execute_on") = execute_options;
    _problem->addAuxKernel("SelfAux",
                           "SelfAux_" + var_name[i],
                           params);
  }

}
