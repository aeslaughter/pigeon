[Mesh]
  type = GeneratedMesh
  dim = 2
  ny = 3
  xmax = 100
  ymax = 100
[]

[Variables]
  active = 'liquid_pressure'
  [./liquid_saturation]
  [../]
  [./liquid_pressure]
  [../]
[]

[AuxVariables]
  [./liquid_density]
  [../]
  [./liquid_viscosity]
  [../]
  [./liquid_relperm]
  [../]
  [./gas_relperm]
  [../]
  [./gas_pressure]
  [../]
  [./gas_saturation]
  [../]
  [./gas_density]
  [../]
  [./gas_viscosity]
  [../]
  [./liquid_saturation]
    initial_condition = 1
  [../]
  [./xh20liquid]
    initial_condition = 1
  [../]
  [./xh20gas]
  [../]
[]

[Functions]
  [./PressureICFunction]
    type = ParsedFunction
    value = 1.49e6
  [../]
[]

[Kernels]
  active = 'H2OMassTime H2OLiquidFlux'
  [./H2OMassTime]
    type = ComponentMassTimeDerivative
    variable = liquid_pressure
    primary_variable_type = pressure
    fluid_saturation_variables = liquid_saturation
    fluid_density_variables = liquid_density
    component_mass_fraction_variables = xh20liquid
    fluid_state_uo = FluidState
    fluid_pressure_variables = liquid_pressure
  [../]
  [./H2OLiquidFlux]
    type = ComponentFlux
    variable = liquid_pressure
    primary_variable_type = pressure
    fluid_pressure_variables = liquid_pressure
    fluid_state_uo = FluidState
    relperm_variables = liquid_relperm
    fluid_density_variables = liquid_density
    fluid_viscosity_variables = liquid_viscosity
    component_mass_fraction_variables = xh20liquid
  [../]
  [./CO2GasFlux]
    type = ComponentFlux
    variable = liquid_saturation
    fluid_viscosity_variable = gas_viscosity
    fluid_density_variable = gas_density
    primary_variable_type = pressure
    xfluid_variable = 1
    relperm_variable = gas_relperm
    fluid_pressure_variable = gas_pressure
  [../]
  [./CO2MassTime]
    type = ComponentMassTimeDerivative
    variable = liquid_saturation
    liquid_density_variable = liquid_density
    xliquid_variable = 0
    gas_saturation_variable = gas_saturation
    primary_variable_type = liquid_saturation
    gas_density_variable = gas_density
    liquid_saturation_variable = liquid_saturation
    fluid_pressure_variables = liquid_pressure
    xgas_variable = 1
  [../]
[]

[AuxKernels]
  [./LiquidDensityAux]
    type = FluidStateAux
    variable = liquid_density
    state_property_enum = density
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = liquid_pressure
  [../]
  [./LiquidViscosityAux]
    type = FluidStateAux
    variable = liquid_viscosity
    state_property_enum = viscosity
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = liquid_pressure
  [../]
  [./LiquidRelPermAux]
    type = FluidStateAux
    variable = liquid_relperm
    state_property_enum = relperm
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = liquid_pressure
    saturation_variable = liquid_saturation
  [../]
  [./GasDensityAux]
    type = FluidStateAux
    variable = gas_density
    saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = density
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./GasPressureAux]
    type = FluidStateAux
    variable = gas_pressure
    saturation_variable = liquid_saturation
    state_property_enum = pressure
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = liquid_pressure
  [../]
  [./GasSaturationAux]
    type = FluidStateAux
    variable = gas_saturation
    saturation_variable = liquid_saturation
    state_property_enum = saturation
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
    phase_index = 1
  [../]
  [./GasViscosityAux]
    type = FluidStateAux
    variable = gas_viscosity
    saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = viscosity
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./GasRelPermAux]
    type = FluidStateAux
    variable = gas_relperm
    saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = relperm
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
[]

[Materials]
  active = 'PorousMaterial'
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    rel_perm_uo = RelativePermeabilityVanGenuchten
    density = 2600.
    cap_pres_uo = CapillaryPressure
    gravity = '0 9.8 0'
    liquid_saturation_variable = 1
    diffusivity = 0.
    permeability = '1e-12 0 0 0 1e-12 0 0 0 1e-12'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    liquid_saturation_variable = liquid_saturation
    temperature_variable = temperature
    gas_pressure_variable = gas_pressure
    liquid_pressure_variable = liquid_pressure
    fluid_state_uo = FluidState
  [../]
[]

[Postprocessors]
  [./h20mass]
    type = ComponentMassPostprocessor
    fluid_density_variable = liquid_density
    phase_saturation_variable = liquid_saturation
  [../]
[]

[UserObjects]
  active = 'FluidState RelativePermeabilityVanGenuchten FluidPropertiesWater CapillaryPressure'
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
    execute_on = 'initial linear'
  [../]
  [./FluidPropertiesCO2]
    type = FluidPropertiesCO2
    execute_on = 'initial linear'
  [../]
  [./CapillaryPressure]
    type = CapillaryPressureVanGenuchten
    p0 = 1e3
    cp_max = 1e4
    m = 0.5
    sat_ls = 1.0
    sat_lr = 0.2
    execute_on = 'initial linear'
  [../]
  [./RelativePermeabilityVanGenuchten]
    type = RelativePermeabilityVanGenuchten
    sat_gr = 0.25
    m = 0.75
    sat_ls = 1
    sat_lr = 0.5
    execute_on = 'initial linear'
  [../]
  [./FluidState]
    type = FluidStateSinglePhase
    execute_on = 'initial linear'
    fluid_temperature = 50
    isothermal = true
    fluid_property_uo = FluidPropertiesWater
  [../]
[]

[Preconditioning]
  active = 'fdp'
  [./fdp]
    type = FDP
    full = true
  [../]
  [./smp]
    type = SMP
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  end_time = 1e6
  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-12
  trans_ss_check = true
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 5
    growth_factor = 1.1
  [../]
[]

[Outputs]
  output_initial = true
  print_linear_residuals = true
  print_perf_log = true
  [./ex]
    type = Exodus
    output_on = 'TIMESTEP_END failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed failed'
  [../]
  [./con]
    output_linear = true
    type = Console
  [../]
[]

[ICs]
  active = 'PressureIC'
  [./LiquidSaturationIC]
    variable = liquid_saturation
    type = ConstantIC
    value = 0.7
  [../]
  [./PressureIC]
    function = PressureICFunction
    variable = liquid_pressure
    type = FunctionIC
  [../]
[]
