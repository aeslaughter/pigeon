# #############################################################################
# Test the FluidPropertiesMethane UserObject properties:
#
# * density - the density of CH4 as a function of pressure and temperature.
# * viscosity - the viscosity of CH4 as a function of temperature.
#
# For density, the pressure is set to vary linearly from 1MPa at the left
# end of the mesh to 10MPa at the right end. Temperature is varied from 50 C
# to 200 C for each test.
#
# #############################################################################

[UserObjects]
  [./FluidPropertiesMethane]
    type = FluidPropertiesMethane
    execute_on = initial
  [../]
[]

[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = 5.0e6*x
  [../]
  [./initial_temperature]
    type = ParsedFunction
    value = 100.
  [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
  xmax = 2
[]

[AuxVariables]
  [./MethanePropertyAuxVariable]
  [../]
[]

[AuxKernels]
  [./MethanePropertyAux]
    type = FluidPropertyMethaneAux
    variable = MethanePropertyAuxVariable
    ch4_property_uo = FluidPropertiesMethane
    ch4_property_enum = density
    temperature_variable = temperature
    execute_on = initial
    pressure_variable = pressure
  [../]
[]

[Variables]
  [./pressure]
  [../]
  [./temperature]
  [../]
[]

[Kernels]
  [./Diffusion]
    type = Diffusion
    variable = pressure
  [../]
[]

[VectorPostprocessors]
  [./vpp]
    type = NodalValueSampler
    variable = MethanePropertyAuxVariable
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
  file_base = MethaneProperty_density
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
  [./TemperatureIC]
    function = initial_temperature
    variable = temperature
    type = FunctionIC
  [../]
[]
