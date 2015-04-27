#include "QuollApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

// Kernels
#include "TwoDConvectionDiffusionDDC.h"
#include "TwoDDarcyDDC.h"
#include "ThreeDConvectionDiffusionDDC.h"
#include "ThreeDDarcyDDCx.h"
#include "ThreeDDarcyDDCy.h"
#include "MassTimeDerivative1.h"
#include "MassTimeDerivative2.h"
#include "Flux1.h"
#include "Flux2.h"
#include "SinglePhaseFlux1.h"
#include "SinglePhaseFlux2.h"
#include "SinglePhaseMassTimeDerivative1.h"
#include "SinglePhaseMassTimeDerivative2.h"
#include "MultiphaseFluxN.h"
#include "MultiphaseFluxW.h"
#include "MultiphaseMassTimeDerivativeN.h"
#include "MultiphaseMassTimeDerivativeW.h"
#include "ComponentMassTimeDerivative.h"
#include "ComponentFlux.h"

// Initial conditions
#include "PerturbationIC.h"

// Boundary conditions
#include "ConstantMassFluxBC.h"

// Auxillary kernels
#include "SaturationAux.h"
#include "CapillaryPressureAux.h"
#include "GasPressureAux.h"
#include "FluidDensityAux.h"
#include "FluidViscosityAux.h"
#include "FluidPropertyAux.h"
#include "RelativePermeabilityAux.h"

// Mesh modifiers
#include "VerticalRefine.h"

// Materials
#include "PorousMaterial.h"

// User object
// Relative permeability
#include "RelativePermeabilityVanGenuchten.h"
#include "RelativePermeabilityCorey.h"
#include "RelativePermeabilityPerfectlyMobile.h"
// Capillary pressure
#include "CapillaryPressureVanGenuchten.h"
#include "CapillaryPressureConstant.h"
// Fluid properties
#include "FluidPropertiesCO2.h"
#include "FluidPropertiesWater.h"

// Postprocessors
#include "ComponentMassPostprocessor.h"
#include "SideMassFluxIntegralPostprocessor.h"

// Dirac kernels
#include "TimeLimitedConstantPointSource.h"

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
// registerKernel(TwoDConvectionDiffusionDDC);
// registerKernel(TwoDDarcyDDC);
// registerKernel(ThreeDConvectionDiffusionDDC);
// registerKernel(ThreeDDarcyDDCx);
// registerKernel(ThreeDDarcyDDCy);
   registerKernel(MassTimeDerivative1);
   registerKernel(MassTimeDerivative2);
   registerKernel(Flux1);
   registerKernel(Flux2);
   registerKernel(SinglePhaseFlux1);
   registerKernel(SinglePhaseFlux2);
   registerKernel(SinglePhaseMassTimeDerivative1);
   registerKernel(SinglePhaseMassTimeDerivative2);
   registerKernel(MultiphaseFluxN);
   registerKernel(MultiphaseFluxW);
   registerKernel(MultiphaseMassTimeDerivativeN);
   registerKernel(MultiphaseMassTimeDerivativeW);
   registerKernel(ComponentMassTimeDerivative);
   registerKernel(ComponentFlux);

   // Register the pertubation initial condition to seed the instability
// registerInitialCondition(PerturbationIC);

   // Register the boundary conditions
   registerBoundaryCondition(ConstantMassFluxBC);
   
   // Register the auxillary kernel to calculate the fluid velocities from the streamfunction
   // registerAux(VelocityAux);
   registerAux(SaturationAux);
   registerAux(CapillaryPressureAux);
   registerAux(GasPressureAux);
   registerAux(FluidDensityAux);
   registerAux(FluidViscosityAux);
   registerAux(RelativePermeabilityAux);
   registerAux(FluidPropertyAux);

   // Register the mesh modifier to rescale the vertical component so that the mesh is refined at the top
// registerMeshModifier(VerticalRefine);
 
   // Register the porous material
   registerMaterial(PorousMaterial);

   // Register user objects for relative permeability and capillary pressure
   registerUserObject(RelativePermeabilityVanGenuchten);
   registerUserObject(RelativePermeabilityCorey);
   registerUserObject(RelativePermeabilityPerfectlyMobile);
   registerUserObject(CapillaryPressureVanGenuchten);
   registerUserObject(CapillaryPressureConstant);

   // Register user object for fluid properties
   registerUserObject(FluidPropertiesCO2);
   registerUserObject(FluidPropertiesWater);

   // Register postprocessors
   registerPostprocessor(ComponentMassPostprocessor);
   registerPostprocessor(SideMassFluxIntegralPostprocessor);

   // Register Dirac kernel
   registerDiracKernel(TimeLimitedConstantPointSource);

}

void
QuollApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
