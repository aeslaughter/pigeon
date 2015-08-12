#include "MosquitoApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
// Stuff Added for neutronic solve example
#include "NeutronicDiffusion.h"
#include "NeutronicRemoval.h"
#include "NeutronicSource.h"
#include "DiffusionNeutronicsMaterial.h"
// End neutronic example headers

template<>
InputParameters validParams<MosquitoApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

MosquitoApp::MosquitoApp(InputParameters parameters) :
    MooseApp(parameters)
{

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  MosquitoApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  MosquitoApp::associateSyntax(_syntax, _action_factory);
}

MosquitoApp::~MosquitoApp()
{
}

// External entry point for dynamic application loading
extern "C" void MosquitoApp__registerApps() { MosquitoApp::registerApps(); }
void
MosquitoApp::registerApps()
{
  registerApp(MosquitoApp);
}

// External entry point for dynamic object registration
extern "C" void MosquitoApp__registerObjects(Factory & factory) { MosquitoApp::registerObjects(factory); }
void
MosquitoApp::registerObjects(Factory & factory)
{
  // Stuff Added for neutronic solve example
  registerMaterial(DiffusionNeutronicsMaterial);
  registerKernel(NeutronicDiffusion);
  registerKernel(NeutronicRemoval);
  registerKernel(NeutronicSource);
  // End neutronic example kernels
}

// External entry point for dynamic syntax association
extern "C" void MosquitoApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { MosquitoApp::associateSyntax(syntax, action_factory); }
void
MosquitoApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
