#include "SquirrelApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<SquirrelApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

SquirrelApp::SquirrelApp(InputParameters parameters) :
    MooseApp(parameters)
{

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  SquirrelApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  SquirrelApp::associateSyntax(_syntax, _action_factory);
}

SquirrelApp::~SquirrelApp()
{
}

extern "C" void SquirrelApp__registerApps() { SquirrelApp::registerApps(); }
void
SquirrelApp::registerApps()
{
  registerApp(SquirrelApp);
}

void
SquirrelApp::registerObjects(Factory & factory)
{
}

void
SquirrelApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
