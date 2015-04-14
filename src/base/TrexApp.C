#include "TrexApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<TrexApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

TrexApp::TrexApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  TrexApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  TrexApp::associateSyntax(_syntax, _action_factory);
}

TrexApp::~TrexApp()
{
}

extern "C" void TrexApp__registerApps() { TrexApp::registerApps(); }
void
TrexApp::registerApps()
{
  registerApp(TrexApp);
}

void
TrexApp::registerObjects(Factory & factory)
{
}

void
TrexApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
