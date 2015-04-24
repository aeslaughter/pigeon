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
  [./LiquidRelativePermeabilityAux]
  [../]
  [./GasRelativePermeabilityAux]
  [../]
[]

[AuxKernels]
  [./LiquidRelativePermeabilityAux]
    type = RelativePermeabilityAux
    variable = LiquidRelativePermeabilityAux
    fluid_phase_enum = liquid
    relative_permeability_uo = RelativePermeability
    liquid_saturation_variable = sat_liq
    execute_on = initial
  [../]
  [./GasRelativePermeabilityAux]
    type = RelativePermeabilityAux
    variable = GasRelativePermeabilityAux
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
    variable = 'GasRelativePermeabilityAux LiquidRelativePermeabilityAux'
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

