# #############################################################################
# Test the FluidPropertiesWater UserObject properties:
# * tSat - the saturated water temperature as a function of pressure.
# * pSat - the saturated water pressure as a function of temperature.
# * fluidDensity - the density of water as a function of pressure and temperature.
#
# For tSat, the fluid pressure is set to vary from 0.1 MPa at the left end of
# the 1D mesh to 10 MPa at the right end. Out of range exceptions are also
# tested.
#
# For pSat, the temperature is set to vary from 10 C at the left end of the 1D
# mesh to 350 at the right end. Out of range exceptions are also tested.
#
# For fluidDensity, the pressure is set to vary linearly from 1MPa at the left
# end of the mesh to 20MPa at the right end. Temperature is varied from 50 C
# to 200 C for each test.
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
    value = 0.1e6*(10.^x)
  [../]
  [./initial_temperature]
    type = ParsedFunction
    value = 10+170*x
  [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
  xmax = 2
[]

[AuxVariables]
  [./WaterPropertyAuxVariable]
  [../]
[]

[AuxKernels]
  [./FluidPropertyAux]
    type = FluidPropertyWaterAux
    variable = WaterPropertyAuxVariable
    fluid_property_enum = tsat
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
    variable = WaterPropertyAuxVariable
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
  file_base = WaterProperty_tSat
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

