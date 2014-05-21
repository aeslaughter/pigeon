#include "LeleApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<LeleApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

LeleApp::LeleApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  LeleApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  LeleApp::associateSyntax(_syntax, _action_factory);
}

LeleApp::~LeleApp()
{
}

void
LeleApp::registerApps()
{
  registerApp(LeleApp);
}

void
LeleApp::registerObjects(Factory & factory)
{
}

void
LeleApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
