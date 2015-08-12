#include "AnteaterApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<AnteaterApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

AnteaterApp::AnteaterApp(InputParameters parameters) :
    MooseApp(parameters)
{

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  AnteaterApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  AnteaterApp::associateSyntax(_syntax, _action_factory);
}

AnteaterApp::~AnteaterApp()
{
}

extern "C" void AnteaterApp__registerApps() { AnteaterApp::registerApps(); }
void
AnteaterApp::registerApps()
{
  registerApp(AnteaterApp);
}

void
AnteaterApp::registerObjects(Factory & factory)
{
}

void
AnteaterApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
