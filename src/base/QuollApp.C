#include "QuollApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

// Kernels
#include "ComponentMassTimeDerivative.h"
#include "ComponentFlux.h"
//#include "LumpedMassChange.h"
//#include "FullyUpwindFlux.h"

// Boundary conditions
#include "ConstantMassFluxBC.h"

// Auxillary kernels
#include "SaturationAux.h"
#include "CapillaryPressureAux.h"
#include "FluidPressureAux.h"
#include "FluidPropertyWaterAux.h"
#include "FluidPropertyBrineAux.h"
#include "FluidPropertyCO2Aux.h"
#include "FluidPropertyMethaneAux.h"
#include "RelativePermeabilityAux.h"
#include "FluidStateAux.h"

// Materials
#include "PorousMaterial.h"
#include "FluidStateMaterial.h"
#include "tmpMaterial.h"
#include "SimpleFluidStateMaterial.h"

// User object
// Relative permeability
#include "RelativePermeabilityVanGenuchten.h"
#include "RelativePermeabilityCorey.h"
#include "RelativePermeabilityPerfectlyMobile.h"
#include "SimpleRelPermVG.h"
// Capillary pressure
#include "CapillaryPressureVanGenuchten.h"
#include "CapillaryPressureConstant.h"
// Fluid properties
#include "FluidPropertiesCO2.h"
#include "FluidPropertiesWater.h"
#include "FluidPropertiesBrine.h"
#include "FluidPropertiesIdealGas.h"
#include "FluidPropertiesMethane.h"

// Fluid states
#include "FluidStateSinglePhase.h"
//#include "FluidStateBrineCO2.h"
//#include "FluidStateWaterCO2.h"
#include "FluidStateTwoPhase.h"

// Postprocessors
#include "ComponentMassPostprocessor.h"
#include "SideMassFluxIntegralPostprocessor.h"

// Dirac kernels
#include "TimeLimitedConstantPointSource.h"

// Problems
#include "MultiphaseProblem.h"

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

   // Register the kernels
   registerKernel(ComponentMassTimeDerivative);
   registerKernel(ComponentFlux);
   registerKernel(LumpedMassChange);
   registerKernel(FullyUpwindFlux);

   // Register the boundary conditions
   registerBoundaryCondition(ConstantMassFluxBC);

   // Register the auxillary kernels
   registerAux(SaturationAux);
   registerAux(CapillaryPressureAux);
   registerAux(FluidPressureAux);
   registerAux(RelativePermeabilityAux);
   registerAux(FluidPropertyWaterAux);
   registerAux(FluidPropertyBrineAux);
   registerAux(FluidPropertyCO2Aux);
   registerAux(FluidPropertyMethaneAux);
   registerAux(FluidStateAux);

   // Register the materials
   registerMaterial(PorousMaterial);
   registerMaterial(FluidStateMaterial);
   registerMaterial(tmpMaterial);
   registerMaterial(SimpleFluidStateMaterial);

   // Register user objects for relative permeability and capillary pressure
   registerUserObject(RelativePermeabilityVanGenuchten);
   registerUserObject(RelativePermeabilityCorey);
   registerUserObject(RelativePermeabilityPerfectlyMobile);
   registerUserObject(CapillaryPressureVanGenuchten);
   registerUserObject(CapillaryPressureConstant);
   registerUserObject(SimpleRelPermVG);

   // Register user object for fluid properties
   registerUserObject(FluidPropertiesCO2);
   registerUserObject(FluidPropertiesWater);
   registerUserObject(FluidPropertiesBrine);
   registerUserObject(FluidPropertiesIdealGas);
   registerUserObject(FluidPropertiesMethane);

   // Register user objects for fluid states
   registerUserObject(FluidStateSinglePhase);
  // registerUserObject(FluidStateBrineCO2);
  // registerUserObject(FluidStateWaterCO2);
   registerUserObject(FluidStateTwoPhase);

   // Register postprocessors
   registerPostprocessor(ComponentMassPostprocessor);
   registerPostprocessor(SideMassFluxIntegralPostprocessor);

   // Register Dirac kernel
   registerDiracKernel(TimeLimitedConstantPointSource);

   // Register problem
   registerProblem(MultiphaseProblem);

}

void
QuollApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
