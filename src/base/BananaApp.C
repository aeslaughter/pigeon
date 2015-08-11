#include "BananaApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<BananaApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

BananaApp::BananaApp(InputParameters parameters) :
    MooseApp(parameters)
{

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  BananaApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  BananaApp::associateSyntax(_syntax, _action_factory);
}

BananaApp::~BananaApp()
{
}

// External entry point for dynamic application loading
extern "C" void BananaApp__registerApps() { BananaApp::registerApps(); }
void
BananaApp::registerApps()
{
  registerApp(BananaApp);
}

// External entry point for dynamic object registration
extern "C" void BananaApp__registerObjects(Factory & factory) { BananaApp::registerObjects(factory); }
void
BananaApp::registerObjects(Factory & factory)
{
}

// External entry point for dynamic syntax association
extern "C" void BananaApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { BananaApp::associateSyntax(syntax, action_factory); }
void
BananaApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
