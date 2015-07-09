# #############################################################################
# Test the FluidPropertiesCO2 UserObject properties:
#
# * density - the density of CO2 as a function of pressure and temperature.
# * viscosity - the viscosity of CO2 as a function of pressure and temperature.
#
# For density, the pressure is set to vary linearly from 1MPa at the left
# end of the mesh to 20MPa at the right end. Temperature is varied from 50 C
# to 200 C for each test. Salt mass fraction is constant.
#
# #############################################################################

[UserObjects]
  [./FluidPropertiesCO2]
    type = FluidPropertiesCO2
    execute_on = initial
  [../]
[]

[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = 1e6*(1+9.5*x)
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
  [./CO2PropertyAuxVariable]
  [../]
[]

[AuxKernels]
  [./CO2PropertyAux]
    type = FluidPropertyCO2Aux
    variable = CO2PropertyAuxVariable
    co2_property_uo = FluidPropertiesCO2
    co2_property_enum = density
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
    variable = CO2PropertyAuxVariable
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
  file_base = CO2Property_density
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
