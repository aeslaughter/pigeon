#include "KktestApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<KktestApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

KktestApp::KktestApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  KktestApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  KktestApp::associateSyntax(_syntax, _action_factory);
}

KktestApp::~KktestApp()
{
}

extern "C" void KktestApp__registerApps() { KktestApp::registerApps(); }
void
KktestApp::registerApps()
{
  registerApp(KktestApp);
}

void
KktestApp::registerObjects(Factory & factory)
{
}

void
KktestApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
