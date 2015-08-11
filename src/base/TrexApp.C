#include "TrexApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"



// Auxkernels
#include "GrainRadiusAux.h"

// Kernels
#include "CalcDiffusion.h"
#include "NeutronHeatSource.h"
#include "SplitCHPoly.h"

// Materials
#include "CalcDiffusionCoef.h"
#include "GBChange.h"

   ///Marmot///
   #include "ConcEvolution.h"
   #include "Porosity.h"

   //Bison//
   #include "FastNeutronFluxAux.h"
   #include "FastNeutronFluenceAux.h"
   #include "ThermalFuel.h"
   #include "VSwellingUO2.h"
//   #include "CreepUO2.h"
//   #include "RelocationUO2.h"
//   #include "MaterialUO2.h"
//   #include "MechZry.h"
   #include "Sifgrs.h"
   #include "ElementIntegralFisGasGeneratedSifgrs.h"
   #include "ElementIntegralFisGasReleasedSifgrs.h"
   #include "ElementIntegralFisGasGrainSifgrs.h"
   #include "BurnupFunction.h"
   #include "ArrheniusDiffusion.h"
   #include "Decay.h"
   #include "RadioActiveDecayConstant.h"
   #include "ArrheniusDiffusionCoef.h"
   #include "CreepPyC.h"
   #include "BurnupAux.h"
   #include "FissionRateAux.h"

    ///Moose Examples///
#include "ExampleDiffusion.h"
#include "BlockAverageDiffusionMaterial.h"
#include "BlockAverageValue.h"




template<>
InputParameters validParams<TrexApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

TrexApp::TrexApp(InputParameters parameters) :
    MooseApp(parameters)
{

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

void
TrexApp::registerApps()
{
  registerApp(TrexApp);
}

void
TrexApp::registerObjects(Factory & factory)
{

  //AuxKernels
  registerAux(GrainRadiusAux);
  
  //Kernels
  registerKernel(CalcDiffusion);
  registerKernel(NeutronHeatSource);
  registerKernel(SplitCHPoly);
  
  
  //Materials
  registerMaterial(CalcDiffusionCoef);
  registerMaterial(GBChange);
  

      //Marmot//
      registerMaterial(ConcEvolution);
      registerPostprocessor(Porosity);

//Moose Examples//
registerKernel(ExampleDiffusion);
registerMaterial(BlockAverageDiffusionMaterial);
registerObject(BlockAverageValue);



     //Bison//
     registerAux(FastNeutronFluxAux);
     registerAux(FastNeutronFluenceAux);
     registerAux(BurnupAux);
registerAux(FissionRateAux);

    registerMaterial(ThermalFuel);
     registerMaterial(VSwellingUO2);
//     registerMaterial(CreepUO2);
//     registerMaterial(RelocationUO2);
//     registerMaterial(MaterialUO2);
//     registerMaterial(MechZry);
     registerMaterial(Sifgrs);
     registerPostprocessor(ElementIntegralFisGasGeneratedSifgrs);
     registerPostprocessor(ElementIntegralFisGasReleasedSifgrs);
     registerPostprocessor(ElementIntegralFisGasGrainSifgrs);
     registerFunction(BurnupFunction);
     registerKernel(ArrheniusDiffusion);
     registerKernel(Decay);
     registerMaterial(RadioActiveDecayConstant);
     registerMaterial(ArrheniusDiffusionCoef);
     registerMaterial(CreepPyC);











}

void
TrexApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
