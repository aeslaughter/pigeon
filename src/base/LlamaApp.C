#include "LlamaApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

////////////////////////////////////////////////////////////////
///                     Heat equation                         //
////////////////////////////////////////////////////////////////
#include "ThermalTimeDerivative.h"
#include "ThermalConduction.h"
#include "ThermalAdvection.h"
#include "PressureTimeDerivative.h"
#include "PressureDarcy.h"
#include "PorousMaterial.h"

template<>
InputParameters validParams<LlamaApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

LlamaApp::LlamaApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  LlamaApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  LlamaApp::associateSyntax(_syntax, _action_factory);
}

LlamaApp::~LlamaApp()
{
}

void
LlamaApp::registerApps()
{
  registerApp(LlamaApp);
}

void
LlamaApp::registerObjects(Factory & factory)
{
  // heat equation
  registerKernel(ThermalTimeDerivative);
  registerKernel(ThermalConduction);
  registerKernel(ThermalAdvection);

  // fluid mass balance
  registerKernel(PressureTimeDerivative);
  registerKernel(PressureDarcy);

  // material
  registerMaterial(PorousMaterial);
}

void
LlamaApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
