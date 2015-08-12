#include "TardigradeApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<TardigradeApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

TardigradeApp::TardigradeApp(InputParameters parameters) :
    MooseApp(parameters)
{

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  TardigradeApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  TardigradeApp::associateSyntax(_syntax, _action_factory);
}

TardigradeApp::~TardigradeApp()
{
}

// External entry point for dynamic application loading
extern "C" void TardigradeApp__registerApps() { TardigradeApp::registerApps(); }
void
TardigradeApp::registerApps()
{
  registerApp(TardigradeApp);
}

// External entry point for dynamic object registration
extern "C" void TardigradeApp__registerObjects(Factory & factory) { TardigradeApp::registerObjects(factory); }
void
TardigradeApp::registerObjects(Factory & factory)
{
}

// External entry point for dynamic syntax association
extern "C" void TardigradeApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { TardigradeApp::associateSyntax(syntax, action_factory); }
void
TardigradeApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
