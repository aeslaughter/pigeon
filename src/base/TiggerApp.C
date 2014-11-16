#include "TiggerApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

#include "CoupledHeatSource.h"

#include "ExtendedHeatConductionMaterial.h"

template<>
InputParameters validParams<TiggerApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

TiggerApp::TiggerApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  TiggerApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  TiggerApp::associateSyntax(_syntax, _action_factory);
}

TiggerApp::~TiggerApp()
{
}

void
TiggerApp::registerApps()
{
  registerApp(TiggerApp);
}

void
TiggerApp::registerObjects(Factory & factory)
{
  registerKernel(CoupledHeatSource);
  registerMaterial(ExtendedHeatConductionMaterial);
}

void
TiggerApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
