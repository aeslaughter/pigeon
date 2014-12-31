#include "RavenApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

#include "CHChemPotential.h" 
#include "PFDiffusion.h"
#include "TwoParticleDensityIC.h"

template<>
InputParameters validParams<RavenApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

RavenApp::RavenApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  RavenApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  RavenApp::associateSyntax(_syntax, _action_factory);
}

RavenApp::~RavenApp()
{
}

void
RavenApp::registerApps()
{
  registerApp(RavenApp);
}

void
RavenApp::registerObjects(Factory & factory)
{
  registerKernel(CHChemPotential);
  registerMaterial(PFDiffusion);
  registerInitialCondition(TwoParticleDensityIC);
}

void
RavenApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
