#include "MongooseApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<MongooseApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

MongooseApp::MongooseApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  MongooseApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  MongooseApp::associateSyntax(_syntax, _action_factory);
}

MongooseApp::~MongooseApp()
{
}

void
MongooseApp::registerApps()
{
  registerApp(MongooseApp);
}

void
MongooseApp::registerObjects(Factory & factory)
{
}

void
MongooseApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
