#include "PigeonApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

template<>
InputParameters validParams<PigeonApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

PigeonApp::PigeonApp(InputParameters parameters) :
    MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  PigeonApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  PigeonApp::associateSyntax(_syntax, _action_factory);
}

PigeonApp::~PigeonApp()
{
}

// External entry point for dynamic application loading
extern "C" void PigeonApp__registerApps() { PigeonApp::registerApps(); }
void
PigeonApp::registerApps()
{
  registerApp(PigeonApp);
}

// External entry point for dynamic object registration
extern "C" void PigeonApp__registerObjects(Factory & factory) { PigeonApp::registerObjects(factory); }
void
PigeonApp::registerObjects(Factory & factory)
{
}

// External entry point for dynamic syntax association
extern "C" void PigeonApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { PigeonApp::associateSyntax(syntax, action_factory); }
void
PigeonApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
