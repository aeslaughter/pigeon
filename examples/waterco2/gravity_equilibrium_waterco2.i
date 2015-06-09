[Mesh]
  type = GeneratedMesh
  dim = 2
  ny = 10
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
  active = 'xco2gas xco2liquid xh20liquid liquid_pressure liquid_viscosity gas_density xh20gas liquid_density liquid_relperm gas_relperm gas_saturation gas_viscosity'
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
    initial_condition = 0.9
  [../]
  [./xh20gas]
    initial_condition = 0.1
  [../]
  [./xco2liquid]
  [../]
  [./xco2gas]
  [../]
  [./liquid_pressure]
  [../]
[]

[Functions]
  [./PressureICFunction]
    type = ParsedFunction
    value = 1.5e6
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
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = liquid_pressure
    saturation_variable = liquid_saturation
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
    execute_on = 'initial linear '
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./GasSaturationAux]
    type = FluidStateAux
    variable = gas_saturation
    saturation_variable = liquid_saturation
    state_property_enum = saturation
    execute_on = 'initial linear'
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
    execute_on = 'LINEAR initial '
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./GasRelPermAux]
    type = FluidStateAux
    variable = gas_relperm
    saturation_variable = liquid_saturation
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
    saturation_variable = liquid_saturation
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./Xco2l]
    type = FluidStateAux
    variable = xco2liquid
    component_index = 1
    saturation_variable = liquid_saturation
    state_property_enum = mass_fraction
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./Xco2g]
    type = FluidStateAux
    variable = xco2gas
    component_index = 1
    saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = mass_fraction
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./Xh20g]
    type = FluidStateAux
    variable = xh20gas
    saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = mass_fraction
    execute_on = 'LINEAR initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./Xh2ol]
    type = FluidStateAux
    variable = xh20liquid
    saturation_variable = liquid_saturation
    state_property_enum = mass_fraction
    execute_on = 'LINEAR initial'
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
    permeability = '1e-13 0 0 0 1e-13 0 0 0 1e-13'
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
    execute_on = '  timestep_end'
    saturation_variables = 'liquid_saturation gas_saturation'
    density_variables = 'liquid_density gas_density'
    component_mass_fraction_variables = 'xh20liquid xh20gas'
  [../]
  [./co2mass]
    type = ComponentMassPostprocessor
    saturation_variables = 'liquid_saturation gas_saturation'
    component_mass_fraction_variables = 'xco2liquid  xco2gas'
    density_variables = 'liquid_density gas_density'
  [../]
  [./co2massg]
    type = ComponentMassPostprocessor
    saturation_variables = gas_saturation
    component_mass_fraction_variables = xco2gas
    density_variables = gas_density
  [../]
  [./co2massl]
    type = ComponentMassPostprocessor
    saturation_variables = liquid_saturation
    component_mass_fraction_variables = xco2liquid
    density_variables = liquid_density
  [../]
[]

[VectorPostprocessors]
  [./gas_pressure]
    type = LineValueSampler
    variable = gas_pressure
    num_points = 10
    start_point = '50 5 0'
    end_point = '50 95 0'
    sort_by = y
  [../]
  [./gas_sat]
    type = LineValueSampler
    variable = gas_saturation
    num_points = 10
    start_point = '50 5 0 '
    end_point = '50 95 0'
    sort_by = y
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
    type = CapillaryPressureConstant
    execute_on = 'initial linear'
    cp = 0
  [../]
  [./RelativePermeabilityVanGenuchten]
    type = RelativePermeabilityPerfectlyMobile
    execute_on = 'initial linear'
  [../]
  [./FluidState]
    type = FluidStateWaterCO2
    capillary_pressure_uo = CapillaryPressure
    co2_property_uo = FluidPropertiesCO2
    water_property_uo = FluidPropertiesWater
    relative_permeability_uo = RelativePermeabilityVanGenuchten
    execute_on = 'initial linear'
    fluid_temperature = 100
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

[Problem]
  type = MultiphaseProblem
  saturation_variable = liquid_saturation
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  end_time = 1e4
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-8
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 100
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
    value = 0.7
  [../]
  [./PressureIC]
    function = PressureICFunction
    variable = gas_pressure
    type = FunctionIC
  [../]
[]

