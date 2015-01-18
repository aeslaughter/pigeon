#include "Mutt1App.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<Mutt1App>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

Mutt1App::Mutt1App(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  Mutt1App::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  Mutt1App::associateSyntax(_syntax, _action_factory);
}

Mutt1App::~Mutt1App()
{
}

void
Mutt1App::registerApps()
{
  registerApp(Mutt1App);
}

void
Mutt1App::registerObjects(Factory & factory)
{
}

void
Mutt1App::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
