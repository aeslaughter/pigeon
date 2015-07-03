[Mesh]
  type = GeneratedMesh
  dim = 2
  ny = 20
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
  active = 'xco2gas xco2liquid xh2oliquid liquid_pressure liquid_viscosity gas_density xh2ogas liquid_density liquid_relperm gas_relperm gas_saturation gas_viscosity'
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
    initial_condition = 0.7
  [../]
  [./xh2oliquid]
    initial_condition = 1
  [../]
  [./xh2ogas]
    initial_condition = 0
  [../]
  [./xco2liquid]
    initial_condition = 0
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
    value = 1.5e6
  [../]
[]

[Kernels]
  [./H2OMassTime]
    type = ComponentMassTimeDerivative
    variable = gas_pressure
    phase_index = 0
    primary_variable_type = pressure
    fluid_state_uo = FluidState
    fluid_density_variable = liquid_density
    fluid_saturation_variable = liquid_saturation
    fluid_pressure_variable = liquid_pressure
    component_mass_fraction_variable = xh2oliquid
  [../]
  [./H2OLiquidFlux]
    type = ComponentFlux
    variable = gas_pressure
    phase_index = 0
    primary_variable_type = pressure
    fluid_state_uo = FluidState
    fluid_density_variable = liquid_density
    fluid_viscosity_variable = liquid_viscosity
    fluid_relperm_variable = liquid_relperm
    fluid_saturation_variable = liquid_saturation
    fluid_pressure_variable = liquid_pressure
    component_mass_fraction_variable = xh2oliquid
  [../]
  [./CO2MassTime]
    type = ComponentMassTimeDerivative
    variable = liquid_saturation
    primary_variable_type = saturation
    fluid_state_uo = FluidState
    phase_index = 1
    fluid_density_variable = gas_density
    fluid_saturation_variable = gas_saturation
    fluid_pressure_variable = gas_pressure
    component_mass_fraction_variable = xco2gas
  [../]
  [./CO2MassFlux]
    type = ComponentFlux
    variable = liquid_saturation
    fluid_state_uo = FluidState
    primary_variable_type = saturation
    fluid_density_variable = gas_density
    fluid_viscosity_variable = gas_viscosity
    phase_index = 1
    component_mass_fraction_variable = xco2gas
    fluid_relperm_variable = gas_relperm
    fluid_saturation_variable = gas_saturation
    fluid_pressure_variable = gas_pressure
  [../]
[]

[AuxKernels]
  [./LiquidDensityAux]
    type = FluidStateAux
    variable = liquid_density
    state_property_enum = density
    execute_on = 'linear initial'
    fluid_state_uo = FluidState
    pressure_variable = liquid_pressure
    saturation_variable = liquid_saturation
  [../]
  [./LiquidViscosityAux]
    type = FluidStateAux
    variable = liquid_viscosity
    state_property_enum = viscosity
    execute_on = 'linear initial'
    fluid_state_uo = FluidState
    pressure_variable = liquid_pressure
  [../]
  [./LiquidRelPermAux]
    type = FluidStateAux
    variable = liquid_relperm
    state_property_enum = relperm
    execute_on = 'linear initial'
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
    execute_on = 'linear initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./GasRelPermAux]
    type = FluidStateAux
    variable = gas_relperm
    saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = relperm
    execute_on = 'linear initial'
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
    execute_on = 'linear initial'
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
    execute_on = 'linear initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./Xh2og]
    type = FluidStateAux
    variable = xh2ogas
    saturation_variable = liquid_saturation
    phase_index = 1
    state_property_enum = mass_fraction
    execute_on = 'linear initial'
    fluid_state_uo = FluidState
    pressure_variable = gas_pressure
  [../]
  [./Xh2ol]
    type = FluidStateAux
    variable = xh2oliquid
    saturation_variable = liquid_saturation
    state_property_enum = mass_fraction
    execute_on = 'linear initial'
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
    temperature_variable = 100
  [../]
[]

[Postprocessors]
  [./h2omass]
    type = ComponentMassPostprocessor
    execute_on = 'timestep_end'
    saturation_variables = 'liquid_saturation gas_saturation'
    density_variables = 'liquid_density gas_density'
    component_mass_fraction_variables = 'xh2oliquid xh2ogas'
  [../]
  [./co2mass]
    type = ComponentMassPostprocessor
    saturation_variables = 'liquid_saturation gas_saturation'
    component_mass_fraction_variables = 'xco2liquid  xco2gas'
    density_variables = 'liquid_density gas_density'
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
    execute_on = 'initial linear'
    p0 = 1e5
    cp_max = 1e6
    m = 0.8
    sat_ls = 1
    sat_lr = 0.3
  [../]
  [./RelativePermeabilityVanGenuchten]
    type = RelativePermeabilityVanGenuchten
    execute_on = 'initial linear'
    sat_gr = 0.2
    m = 0.8
    sat_ls = 1
    sat_lr = 0.3
  [../]
  [./FluidState]
    type = FluidStateMultiphase
    capillary_pressure_uo = CapillaryPressure
    relative_permeability_uo = RelativePermeabilityVanGenuchten
    execute_on = 'initial linear'
    fluid_temperature = 100
    isothermal = true
    gas_property_uo = FluidPropertiesCO2
    liquid_property_uo = FluidPropertiesWater
  [../]
[]

[Preconditioning]
  active = 'fdp'
  [./fdp]
    type = FDP
    full = true
    petsc_options_iname = '-ksp_type -pc_type'
    petsc_options_value = 'bcgs bjacobi'
  [../]
  [./smp]
    type = SMP
  [../]
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 3e6
  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-8
  l_tol = 1e-12
  dtmax = 2e5
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
    output_on = 'timestep_end'
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
