#include "GrizzlyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<GrizzlyApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

GrizzlyApp::GrizzlyApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  GrizzlyApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  GrizzlyApp::associateSyntax(_syntax, _action_factory);
}

GrizzlyApp::~GrizzlyApp()
{
}

void
GrizzlyApp::registerApps()
{
  registerApp(GrizzlyApp);
}

void
GrizzlyApp::registerObjects(Factory & factory)
{
}

void
GrizzlyApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
