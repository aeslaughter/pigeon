#include "QuollApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<QuollApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

QuollApp::QuollApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  QuollApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  QuollApp::associateSyntax(_syntax, _action_factory);
}

QuollApp::~QuollApp()
{
}

void
QuollApp::registerApps()
{
  registerApp(QuollApp);
}

void
QuollApp::registerObjects(Factory & factory)
{
}

void
QuollApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
