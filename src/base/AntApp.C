#include "AntApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<AntApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

AntApp::AntApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  AntApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  AntApp::associateSyntax(_syntax, _action_factory);
}

AntApp::~AntApp()
{
}

// External entry point for dynamic application loading
extern "C" void AntApp__registerApps() { AntApp::registerApps(); }
void
AntApp::registerApps()
{
  registerApp(AntApp);
}

// External entry point for dynamic object registration
extern "C" void AntApp__registerObjects(Factory & factory) { AntApp::registerObjects(factory); }
void
AntApp::registerObjects(Factory & factory)
{
}

// External entry point for dynamic syntax association
extern "C" void AntApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { AntApp::associateSyntax(syntax, action_factory); }
void
AntApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
