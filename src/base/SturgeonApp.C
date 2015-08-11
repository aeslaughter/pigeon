#include "SturgeonApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<SturgeonApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

SturgeonApp::SturgeonApp(InputParameters parameters) :
    MooseApp(parameters)
{

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  SturgeonApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  SturgeonApp::associateSyntax(_syntax, _action_factory);
}

SturgeonApp::~SturgeonApp()
{
}

extern "C" void SturgeonApp__registerApps() { SturgeonApp::registerApps(); }
void
SturgeonApp::registerApps()
{
  registerApp(SturgeonApp);
}

void
SturgeonApp::registerObjects(Factory & factory)
{
}

void
SturgeonApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
