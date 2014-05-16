#include "ZebraApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<ZebraApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

ZebraApp::ZebraApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  ZebraApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  ZebraApp::associateSyntax(_syntax, _action_factory);
}

ZebraApp::~ZebraApp()
{
}

void
ZebraApp::registerApps()
{
  registerApp(ZebraApp);
}

void
ZebraApp::registerObjects(Factory & factory)
{
}

void
ZebraApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
