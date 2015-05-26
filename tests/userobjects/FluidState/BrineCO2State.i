# #############################################################################
# Test the FluidStateBrineCO2 UserObject properties:
# 
# * liquidDensity - the density of liquid as a function of pressure, temperature
#                   and mass fraction
# * gasDensity - the density of gas as a function of pressure, temperature and
#                mass fraction
# * liquidViscosity - the viscosity of liquid as as a function of pressure, temperature
#                     and mass fraction
# * gasViscosity - the viscosity of gas as as a function of pressure, temperature
#                  and mass fraction
# 
# #############################################################################

[UserObjects]
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
    execute_on = initial
  [../]
  [./FluidPropertiesBrine]
    type = FluidPropertiesBrine
    execute_on = initial
    water_property_uo = FluidPropertiesWater
  [../]
  [./FluidPropertiesCO2]
    type = FluidPropertiesCO2
    execute_on = initial
  [../]
  [./FluidStateBrineCO2]
    type = FluidStateBrineCO2
    execute_on = initial
    brine_property_uo = FluidPropertiesBrine
    co2_property_uo = FluidPropertiesCO2
  [../]
[]

[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = 1.e6
  [../]
  [./initial_temperature]
    type = ParsedFunction
    value = 50.
  [../]
  [./initial_xnacl]
    type = ParsedFunction
    value = 0.125*x
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
  xmax = 2
[]

[AuxVariables]
  [./FluidPropertyAuxVariable]
  [../]
[]

[AuxKernels]
  [./FluidStateAux]
    type = FluidStateAux
    variable = FluidPropertyAuxVariable
    fluid_state_uo = FluidStateBrineCO2
    state_property_enum = gas_density
    salt_mass_fraction_variable = xnacl
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
  [./xnacl]
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
    variable = FluidPropertyAuxVariable
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
  file_base = FluidProperty_liquidDensity
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
  [./XnaclIC]
    function = initial_xnacl
    variable = xnacl
    type = FunctionIC
  [../]
[]

