# #############################################################################
# Test the FluidPropertiesBrine UserObject properties:
#
# * density - the density of water as a function of pressure and temperature.
#
# For density, the pressure is set to vary linearly from 1MPa at the left
# end of the mesh to 20MPa at the right end. Temperature is varied from 50 C
# to 200 C for each test. Salt mass fraction is constant.
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
[]

[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = 20.e6
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
  [./BrinePropertyAuxVariable]
  [../]
[]

[AuxKernels]
  [./BrinePropertyAux]
    type = FluidPropertyBrineAux
    variable = BrinePropertyAuxVariable
    brine_property_enum = density
    salt_mass_fraction_variable = xnacl
    brine_property_uo = FluidPropertiesBrine
    water_property_uo = FluidPropertiesWater
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
    variable = BrinePropertyAuxVariable
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
  file_base = BrineProperty_density
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

