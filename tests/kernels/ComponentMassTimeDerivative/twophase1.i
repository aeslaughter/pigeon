# Tests the on-diagonal and off-diagonal Jacobian terms for pressure and saturation

[Mesh]
  type = GeneratedMesh
  dim = 2
  ny = 5
  nx = 5
  xmax = 100
  ymax = 100
[]

[Variables]
  [./liquid_pressure]
  [../]
  [./liquid_saturation]
  [../]
[]

[AuxVariables]
  [./liquid_density]
  [../]
  [./gas_density]
  [../]
  [./gas_saturation]
  [../]
[]

[Kernels]
  active = 'ComponentMassTimeDerivative1 ComponentMassTimeDerivative2'
  [./ComponentMassTimeDerivative2]
    type = ComponentMassTimeDerivative
    variable = liquid_pressure
    component_index = 0
    fluid_state_uo = FluidState
    fluid_density_variable = liquid_density
    fluid_saturation_variable = liquid_saturation
    component_mass_fraction_variable = 0
    phase_index = 0
  [../]
  [./ComponentMassTimeDerivative1]
    type = ComponentMassTimeDerivative
    variable = liquid_saturation
    component_index = 0
    fluid_state_uo = FluidState
    fluid_density_variable = gas_density
    fluid_saturation_variable = gas_saturation
    component_mass_fraction_variable = 0
    phase_index = 1
  [../]
[]

[AuxKernels]
  [./LiquidDensityAux]
    type = FluidStateAux
    variable = liquid_density
    state_property_enum = density
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./GasDensityAux]
    type = FluidStateAux
    variable = gas_density
    state_property_enum = density
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasSaturationAux]
    type = FluidStateAux
    variable = gas_saturation
    state_property_enum = saturation
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    density = 2600.
    gravity = '0 -10 0'
    liquid_saturation_variable = liquid_saturation
    permeability = '1e-12 0 0 0 1e-12 0 0 0 1e-12'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    block = 0
    fluid_state_uo = FluidState
    diffusivity = '1e-5'
    primary_pressure_variable = liquid_pressure
    primary_saturation_variable = liquid_saturation
    temperature_variable = 100
    fluid_density_variables = 'liquid_density gas_density'
    primary_mass_fraction_variable = 1
  [../]
[]

[UserObjects]
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
    execute_on = initial
  [../]
  [./FluidPropertiesCO2]
    type = FluidPropertiesCO2
  [../]
  [./RelativePermeability]
    type = RelativePermeabilityVanGenuchten
    m = 0.5
    sat_gr = 0
    sat_lr = 0
    sat_ls = 1
  [../]
  [./CapillaryPressure]
    type = CapillaryPressureVanGenuchten
    sat_lr = 0
    sat_ls = 1
    m = 0.5
    cp_max = 1e6
    p0 = 1e5
  [../]
  [./FluidState]
    type = FluidStateTwoPhase
    execute_on = 'initial'
    liquid_property_uo = FluidPropertiesWater
    gas_property_uo = FluidPropertiesCO2
    pressure_variable = liquid_pressure
    saturation_variable = liquid_saturation
    temperature_variable = 100
    component_index = 0
    pressure_variable_phase = 0
    saturation_variable_phase = 0
    capillary_pressure_uo = CapillaryPressure
    relative_permeability_uo = RelativePermeability
  [../]
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 1
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Outputs]
  output_initial = true
  print_linear_residuals = true
  print_perf_log = true
[]

[ICs]
  [./PressureIC]
    type = FunctionIC
    variable = liquid_pressure
    function = 1e6*(2-y/100)
  [../]
  [./SaturationIC]
    type = FunctionIC
    variable = liquid_saturation
    function = 0.5
  [../]
[]

[BCs]
  [./SaturationTop]
    type = DirichletBC
    value = 1
    boundary = top
    variable = liquid_saturation
  [../]
[]
