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

GrizzlyApp::GrizzlyApp(InputParameters parameters) :
    MooseApp(parameters)
{

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

extern "C" void GrizzlyApp__registerApps() { GrizzlyApp::registerApps(); }
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
