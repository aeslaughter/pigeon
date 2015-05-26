# #############################################################################
# Test the FluidStateWaterCO2 UserObject properties:
# 
# * liquidDensity - the density of liquid as a function of pressure and temperature
# * gasDensity - the density of gas as a function of pressure and temperature
# * liquidViscosity - the viscosity of liquid as as a function of pressure and temperature
# * gasViscosity - the viscosity of gas as as a function of pressure and temperature
# * massFractions - the mass fractions of each component in each phase
# 
# #############################################################################

[UserObjects]
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
    execute_on = initial
  [../]
  [./FluidPropertiesCO2]
    type = FluidPropertiesCO2
    execute_on = initial
  [../]
  [./RelativePermeability]
    type = RelativePermeabilityVanGenuchten
    sat_gr = 0.0
    m = 0.5
    sat_ls = 1.0
    sat_lr = 0.0
    execute_on = initial
  [../]
  [./CapillaryPressure]
    type = CapillaryPressureVanGenuchten
    sat_lr = 0.0
    sat_ls = 1.0
    m = 0.5
    cp_max = 1e5
    p0 = 1e4
    execute_on = initial
  [../] 
  [./FluidStateWaterCO2]
    type = FluidStateWaterCO2
    execute_on = initial
    water_property_uo = FluidPropertiesWater
    co2_property_uo = FluidPropertiesCO2
    relative_permeability_uo = RelativePermeability
    capillary_pressure_uo = CapillaryPressure
  [../]
[]

[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = 1.e6*(1+10*x)
  [../]
  [./initial_temperature]
    type = ParsedFunction
    value = 50.
  [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
  xmax = 2
[]

[AuxVariables]
  [./FluidStateAuxVariable]
  [../]
[]

[AuxKernels]
  [./FluidStateAux]
    type = FluidStateAux
    variable = FluidStateAuxVariable
    fluid_state_uo = FluidStateWaterCO2
    state_property_enum = density
    temperature_variable = temperature
    execute_on = initial
    pressure_variable = pressure
    phase_index = 0
    liquid_saturation_variable = 0.5
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
    variable = FluidStateAuxVariable
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
  file_base = FluidStateWaterCO2_liquidDensity
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
