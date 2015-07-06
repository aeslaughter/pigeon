[Mesh]
  type = GeneratedMesh
  dim = 2
  ny = 10
  xmax = 100
  ymax = 100
[]

[Variables]
  [./liquid_pressure]
  [../]
[]

[AuxVariables]
  [./liquid_density]
  [../]
  [./liquid_viscosity]
  [../]
  [./liquid_saturation]
    initial_condition = 1
  [../]
  [./xh20liquid]
    initial_condition = 1
  [../]
  [./liquid_relperm]
    initial_condition = 1
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
    variable = liquid_pressure
    primary_variable_type = pressure
    fluid_state_uo = FluidState
    fluid_density_variable = liquid_density
    fluid_saturation_variable = liquid_saturation
    fluid_pressure_variable = liquid_pressure
  [../]
  [./H2OLiquidFlux]
    type = ComponentFlux
    variable = liquid_pressure
    primary_variable_type = pressure
    fluid_viscosity_variable = liquid_viscosity
    fluid_density_variable = liquid_density
    component_mass_fraction_variable = xh20liquid
    fluid_relperm_variable = liquid_relperm
    fluid_saturation_variable = liquid_saturation
    fluid_pressure_variable = liquid_pressure
    fluid_state_uo = FluidState
  [../]
[]

[AuxKernels]
  [./LiquidDensityAux]
    type = FluidStateAux
    variable = liquid_density
    state_property_enum = density
    execute_on = 'linear initial'
    fluid_state_uo = FluidState
  [../]
  [./LiquidViscosityAux]
    type = FluidStateAux
    variable = liquid_viscosity
    state_property_enum = viscosity
    execute_on = 'linear initial'
    fluid_state_uo = FluidState
  [../]
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    rel_perm_uo = RelativePermeability
    density = 2600.
    cap_pres_uo = CapillaryPressure
    gravity = '0 -9.8 0'
    liquid_saturation_variable = 1
    diffusivity = 0.
    permeability = '1e-12 0 0 0 1e-12 0 0 0 1e-12'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    block = 0
    primary_pressure_variable = liquid_pressure
    primary_saturation_variable = liquid_saturation
    fluid_state_uo = FluidState
    fluid_density_variables = liquid_density
  [../]
[]

[Postprocessors]
  [./h20mass]
    type = ComponentMassPostprocessor
    component_mass_fraction_variables = xh20liquid
    density_variables = liquid_density
  [../]
[]

[UserObjects]
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
    execute_on = 'initial'
  [../]
  [./CapillaryPressure]
    type = CapillaryPressureConstant
    execute_on = 'initial'
    cp = 0
  [../]
  [./FluidState]
    type = FluidStateSinglePhase
    execute_on = 'initial linear'
    isothermal = true
    fluid_property_uo = FluidPropertiesWater
    pressure_variable = liquid_pressure
    temperature_variable = 50
    saturation_variable = liquid_saturation
  [../]
  [./RelativePermeability]
    type = RelativePermeabilityPerfectlyMobile
    execute_on = 'initial'
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
    full = true
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
    dt = 100
  [../]
[]

[Outputs]
  output_initial = true
  print_linear_residuals = true
  print_perf_log = true
  [./ex]
    type = Exodus
    output_on = TIMESTEP_END
  [../]
  [./con]
    output_linear = true
    type = Console
  [../]
[]

[ICs]
  [./PressureIC]
    function = PressureICFunction
    variable = liquid_pressure
    type = FunctionIC
  [../]
[]
