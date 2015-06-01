[Mesh]
  type = GeneratedMesh
  dim = 2
  xmax = 100
  ymax = 100
[]

[Variables]
  [./liquid_saturation]
  [../]
  [./gas_pressure]
  [../]
[]

[AuxVariables]
  active = 'xco2gas xco2liquid xh20liquid liquid_viscosity gas_density xh20gas liquid_density liquid_relperm gas_relperm gas_saturation gas_viscosity liquid_pressure'
  [./liquid_density]
  [../]
  [./liquid_viscosity]
  [../]
  [./liquid_relperm]
  [../]
  [./gas_relperm]
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
  [./xco2liquid]
  [../]
  [./xco2gas]
    initial_condition = 1
  [../]
  [./liquid_pressure]
  [../]
[]

[Functions]
  [./PressureICFunction]
    type = ParsedFunction
    value = 1.49e6
  [../]
[]

[Kernels]
  [./H2OMassTime]
    type = ComponentMassTimeDerivative
    variable = gas_pressure
    primary_variable_type = pressure
    fluid_saturation_variables = 'liquid_saturation gas_saturation'
    fluid_density_variables = 'liquid_density gas_density'
    component_mass_fraction_variables = 'xh20liquid xh20gas'
    fluid_state_uo = FluidState
    fluid_pressure_variables = 'liquid_pressure gas_pressure'
    phase_index = 1
  [../]
  [./H2OLiquidFlux]
    type = ComponentFlux
    variable = gas_pressure
    primary_variable_type = pressure
    fluid_state_uo = FluidState
    relperm_variables = 'liquid_relperm gas_relperm'
    fluid_density_variables = 'liquid_density gas_density'
    fluid_viscosity_variables = 'liquid_viscosity gas_viscosity'
    component_mass_fraction_variables = 'xh20liquid xh20gas'
    phase_index = 1
  [../]
  [./CO2MassTime]
    type = ComponentMassTimeDerivative
    variable = liquid_saturation
    primary_variable_type = saturation
    fluid_saturation_variables = 'liquid_saturation gas_saturation'
    fluid_density_variables = 'liquid_density gas_density'
    component_mass_fraction_variables = 'xco2liquid xco2gas'
    fluid_state_uo = FluidState
    fluid_pressure_variables = 'liquid_pressure gas_pressure'
  [../]
  [./CO2MassFlux]
    type = ComponentFlux
    variable = liquid_saturation
    fluid_state_uo = FluidState
    relperm_variables = 'liquid_relperm gas_relperm'
    primary_variable_type = saturation
    fluid_density_variables = 'liquid_density gas_density'
    component_mass_fraction_variables = 'xco2liquid xco2gas'
    fluid_viscosity_variables = 'liquid_viscosity gas_viscosity'
  [../]
[]

[AuxKernels]
  [./LiquidDensityAux]
    type = FluidStateAux
    variable = liquid_density
    state_property_enum = density
    execute_on = 'LINEAR initial residual'
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
    liquid_saturation_variable = liquid_saturation
  [../]
  [./GasDensityAux]
    type = FluidStateAux
    variable = gas_density
    liquid_saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = density
    execute_on = 'initial linear '
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./GasSaturationAux]
    type = FluidStateAux
    variable = gas_saturation
    liquid_saturation_variable = liquid_saturation
    state_property_enum = saturation
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
    phase_index = 1
  [../]
  [./GasViscosityAux]
    type = FluidStateAux
    variable = gas_viscosity
    liquid_saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = viscosity
    execute_on = 'LINEAR initial '
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./GasRelPermAux]
    type = FluidStateAux
    variable = gas_relperm
    liquid_saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = relperm
    execute_on = 'LINEAR initial '
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./LiquidPressureAux]
    type = FluidStateAux
    variable = liquid_pressure
    state_property_enum = pressure
    liquid_saturation_variable = liquid_saturation
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    rel_perm_uo = RelativePermeabilityVanGenuchten
    density = 2600.
    cap_pres_uo = CapillaryPressure
    gravity = '0 9.8 0'
    liquid_saturation_variable = liquid_saturation
    diffusivity = 0.
    permeability = '1e-12 0 0 0 1e-12 0 0 0 1e-12'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    block = 0
    fluid_state_uo = FluidState
    phase_index = 1
    primary_pressure_variable = gas_pressure
    primary_saturation_variable = liquid_saturation
  [../]
[]

[Postprocessors]
  [./h20mass]
    type = ComponentMassPostprocessor
    fluid_density_variable = liquid_density
    phase_saturation_variable = liquid_saturation
    execute_on = '  timestep_end'
  [../]
  [./co2mass]
    type = ComponentMassPostprocessor
    fluid_density_variable = gas_density
    component_mass_fraction_variable = xco2gas
    phase_saturation_variable = gas_saturation
    phase_index = 1
  [../]
[]

[UserObjects]
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
    p0 = 1e4
    cp_max = 1e5
    m = 0.5
    sat_ls = 1.0
    sat_lr = 0.2
    execute_on = 'initial linear'
  [../]
  [./RelativePermeabilityVanGenuchten]
    type = RelativePermeabilityVanGenuchten
    sat_gr = 0.2
    m = 0.5
    sat_ls = 1
    sat_lr = 0.2
    execute_on = 'initial linear'
  [../]
  [./FluidState]
    type = FluidStateWaterCO2
    capillary_pressure_uo = CapillaryPressure
    co2_property_uo = FluidPropertiesCO2
    water_property_uo = FluidPropertiesWater
    relative_permeability_uo = RelativePermeabilityVanGenuchten
    execute_on = 'initial linear'
    fluid_temperature = 50
    isothermal = true
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
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-8
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
  [./LiquidSaturationIC]
    variable = liquid_saturation
    type = ConstantIC
    value = 0.6
  [../]
  [./PressureIC]
    function = PressureICFunction
    variable = gas_pressure
    type = FunctionIC
  [../]
[]

