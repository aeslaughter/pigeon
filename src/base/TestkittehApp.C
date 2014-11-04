#include "TestkittehApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<TestkittehApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

TestkittehApp::TestkittehApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  TestkittehApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  TestkittehApp::associateSyntax(_syntax, _action_factory);
}

TestkittehApp::~TestkittehApp()
{
}

void
TestkittehApp::registerApps()
{
  registerApp(TestkittehApp);
}

void
TestkittehApp::registerObjects(Factory & factory)
{
}

void
TestkittehApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
