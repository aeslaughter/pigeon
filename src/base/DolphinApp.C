#include "DolphinApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<DolphinApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

DolphinApp::DolphinApp(InputParameters parameters) :
    MooseApp(parameters)
{

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  DolphinApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  DolphinApp::associateSyntax(_syntax, _action_factory);
}

DolphinApp::~DolphinApp()
{
}

// External entry point for dynamic application loading
extern "C" void DolphinApp__registerApps() { DolphinApp::registerApps(); }
void
DolphinApp::registerApps()
{
  registerApp(DolphinApp);
}

// External entry point for dynamic object registration
extern "C" void DolphinApp__registerObjects(Factory & factory) { DolphinApp::registerObjects(factory); }
void
DolphinApp::registerObjects(Factory & factory)
{
}

// External entry point for dynamic syntax association
extern "C" void DolphinApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { DolphinApp::associateSyntax(syntax, action_factory); }
void
DolphinApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
