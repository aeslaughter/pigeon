# #############################################################################
# Test the FluidPropertiesWater UserObject.
#
# The liquid saturation is initialised to vary linearly from zero at the left
# end of the 1D mesh, to unity at the right end.
#
# #############################################################################

[UserObjects]
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
  [../]
[]

[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = 1e6*(1+x)
  [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
[]

[AuxVariables]
  [./WaterDensityAux]
  [../]
[]

[AuxKernels]
  [./FluidDensityAux]
    type = FluidDensityAux
    variable = WaterDensityAux
    saturation_variable = WaterDensityAux
    fluid_property_uo = FluidPropertiesWater
    temperature_variable = temperature
    execute_on = initial
    pressure_variable = pressure
  [../]
[]

[Variables]
  [./pressure]
  [../]
  [./temperature]
    initial_condition = 20
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
    variable = WaterDensityAux
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
  file_base = WaterDensity
  [./csv]
    type = CSV
  [../]
[]

[ICs]
  [./PressureIC]
    function = initial_pressure
    variable = pressure
    type = FunctionIC
  [../]
[]

