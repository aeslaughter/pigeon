# #############################################################################
# Test the RelativePermeabilityPerfectlyMobile UserObject. This relative
# permeability UserObject returns 1.0 for all liquid saturations.
#
# The liquid saturation is initialised to vary linearly from zero at the left
# end of the 1D mesh, to unity at the right end.
#
# #############################################################################

[UserObjects]
  [./RelativePermeability]
    type = RelativePermeabilityVanGenuchten
    sat_gr = 0.0
    m = 0.5
    sat_ls = 1.0
    sat_lr = 0.0
    execute_on = initial
  [../]
[]

[Functions]
  [./initial_saturation]
    type = ParsedFunction
    value = x
  [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
[]

[AuxVariables]
  [./LiquidRelPermAux]
  [../]
  [./GasRelPermAux]
  [../]
  [./dLiquidRelPermAux]
  [../]
  [./dGasRelPermAux]
  [../]
[]

[AuxKernels]
  [./LiquidRelPermAux]
    type = RelativePermeabilityAux
    variable = LiquidRelPermAux
    property_enum = relperm
    fluid_phase_enum = liquid
    relative_permeability_uo = RelativePermeability
    liquid_saturation_variable = sat_liq
    execute_on = initial
  [../]
  [./GasRelPermAux]
    type = RelativePermeabilityAux
    variable = GasRelPermAux
    property_enum = relperm
    fluid_phase_enum = gas
    relative_permeability_uo = RelativePermeability
    liquid_saturation_variable = sat_liq
    execute_on = initial
  [../]
  [./dLiquidRelPermAux]
    type = RelativePermeabilityAux
    variable = dLiquidRelPermAux
    property_enum = drelperm
    fluid_phase_enum = liquid
    relative_permeability_uo = RelativePermeability
    liquid_saturation_variable = sat_liq
    execute_on = initial
  [../]
  [./dGasRelPermAux]
    type = RelativePermeabilityAux
    variable = dGasRelPermAux
    property_enum = drelperm
    fluid_phase_enum = gas
    relative_permeability_uo = RelativePermeability
    liquid_saturation_variable = sat_liq
    execute_on = initial
  [../]
[]

[Variables]
  [./sat_liq]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./Diffusion]
    type = Diffusion
    variable = sat_liq
  [../]
[]

[VectorPostprocessors]
  [./vpp]
    type = NodalValueSampler
    variable = 'GasRelPermAux LiquidRelPermAux dGasRelPermAux dLiquidRelPermAux'
    sort_by = x
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
[]

[Outputs]
  file_base = RelativePermeabilityVanGenuchten1
  [./csv]
    type = CSV
  [../]
[]

[ICs]
  [./LiquidSaturationIC]
    function = initial_saturation
    variable = sat_liq
    type = FunctionIC
  [../]
[]

