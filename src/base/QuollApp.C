#include "QuollApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "TwoDConvectionDiffusionDDC.h"
#include "TwoDDarcyDDC.h"
#include "ThreeDConvectionDiffusionDDC.h"
#include "ThreeDDarcyDDCx.h"
#include "ThreeDDarcyDDCy.h"
#include "PerturbationIC.h"
// #include "VelocityAux.h"
#include "VerticalRefine.h"

template<>
InputParameters validParams<QuollApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

QuollApp::QuollApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  QuollApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  QuollApp::associateSyntax(_syntax, _action_factory);
}

QuollApp::~QuollApp()
{
}

extern "C" void QuollApp__registerApps() { QuollApp::registerApps(); }
void
QuollApp::registerApps()
{
  registerApp(QuollApp);
}

void
QuollApp::registerObjects(Factory & factory)
{

   // Register the custom convection and Darcy kernels defined for this problem
   registerKernel(TwoDConvectionDiffusionDDC);
   registerKernel(TwoDDarcyDDC);
   registerKernel(ThreeDConvectionDiffusionDDC);
   registerKernel(ThreeDDarcyDDCx);
   registerKernel(ThreeDDarcyDDCy);

   // Register the pertubation initial condition to seed the instability
   registerInitialCondition(PerturbationIC);
   
   // Register the auxillary kernel to calculate the fluid velocities from the streamfunction
// registerAux(VelocityAux);

   // Register the mesh modifier to rescale the vertical component so that the mesh is refined at the top
   registerMeshModifier(VerticalRefine);
}

void
QuollApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
