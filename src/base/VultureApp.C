#include "VultureApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<VultureApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

VultureApp::VultureApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  VultureApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  VultureApp::associateSyntax(_syntax, _action_factory);
}

VultureApp::~VultureApp()
{
}

void
VultureApp::registerApps()
{
  registerApp(VultureApp);
}

void
VultureApp::registerObjects(Factory & factory)
{
}

void
VultureApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
