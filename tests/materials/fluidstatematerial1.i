[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
  xmax = 100
[]

[Problem]
  solve = true
[]

[Variables]
  [./gas_pressure]
  [../]
  [./gas_saturation]
  [../]
[]

[AuxVariables]
  [./liquid_saturation]
  [../]
  [./liquid_pressure]
  [../]
  [./liquid_density]
  [../]
  [./liquid_viscosity]
  [../]
  [./liquid_relperm]
  [../]
  [./gas_density]
  [../]
  [./gas_viscosity]
  [../]
  [./gas_relperm]
  [../]  
  [./liquid_phase_flux]
    family = monomial
    order = constant
  [../]
  [./gas_phase_flux]
    family = monomial
    order = constant
  [../]
  [./grad_liquid_pressure]
    family = monomial
    order = constant
  [../]
  [./grad_gas_pressure]
    family = monomial
    order = constant
  [../]
[]

[Kernels]
  active = 'DiffusionS DiffusionP'
  [./DiffusionS]
    type = Diffusion
    variable = gas_pressure
  [../]
  [./DiffusionP]
    type = Diffusion
    variable = gas_saturation
  [../]
[]

[AuxKernels]
  [./LiquidDensityAux]
    type = FluidStateAux
    variable = liquid_density
    state_property_enum = density
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
  [../]
  [./LiquidViscosityAux]
    type = FluidStateAux
    variable = liquid_viscosity
    state_property_enum = viscosity
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
  [../]
  [./LiquidRelPermAux]
    type = FluidStateAux
    variable = liquid_relperm
    state_property_enum = relperm
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
  [../]
  [./GasDensityAux]
    type = FluidStateAux
    variable = gas_density
    state_property_enum = density
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasViscosityAux]
    type = FluidStateAux
    variable = gas_viscosity
    state_property_enum = viscosity
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasRelPermAux]
    type = FluidStateAux
    variable = gas_relperm
    state_property_enum = relperm
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./LiquidPressureAux]
    type = FluidStateAux
    variable = liquid_pressure
    state_property_enum = pressure
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidSaturationAux]
    type = FluidStateAux
    variable = liquid_saturation
    state_property_enum = saturation
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidPhaseFluxAux]
    type = MaterialStdVectorRealGradientAux
    index = 0
    property = phase_flux_no_mobility
    variable = liquid_phase_flux
  [../]
  [./GasPhaseFluxAux]
    type = MaterialStdVectorRealGradientAux
    index = 1
    property = phase_flux_no_mobility
    variable = gas_phase_flux
  [../]
  [./GradLiquidPressureAux]
    type = VariableGradientComponent
    gradient_variable = liquid_pressure
    variable = grad_liquid_pressure
    component = x
  [../]
  [./GradGasPressureAux]
    type = VariableGradientComponent
    gradient_variable = gas_pressure
    variable = grad_gas_pressure
    component = x
  [../]
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    density = 2600.
    gravity = '0 0 0'
    liquid_saturation_variable = liquid_saturation
    diffusivity = 0.
    permeability = '1e-12 0 0 0 1e-12 0 0 0 1e-12'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    block = 0
    fluid_state_uo = FluidState
    primary_pressure_variable = gas_pressure
    primary_saturation_variable = gas_saturation
    temperature_variable = 50
    fluid_density_variables = 'liquid_density gas_density'
  [../]
[]

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
    type = RelativePermeabilityPerfectlyMobile
    execute_on = initial
  [../]
  [./CapillaryPressure]
    type = CapillaryPressureVanGenuchten
    execute_on = initial
    sat_lr = 0
    sat_ls = 1
    m = 0.5
    cp_max = 1e5
    p0 = 1e4
    delta_s = 0.05
  [../]
  [./FluidState]
    type = FluidStateTwoPhase
    execute_on = 'initial linear'
    isothermal = true
    pressure_variable = gas_pressure
    pressure_variable_phase = 1
    temperature_variable = 50
    saturation_variable = gas_saturation
    saturation_variable_phase = 1
    capillary_pressure_uo = CapillaryPressure
    relative_permeability_uo = RelativePermeability
    gas_property_uo = FluidPropertiesCO2
    liquid_property_uo = FluidPropertiesWater
  [../]
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 1
  dt = 1
  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-12
  l_tol = 1e-12
  dtmax = 1e5
[]

[Outputs]
  file_base = fluidstatematerial1
  csv = true
  output_initial = false
  [./con]
    output_linear = true
    type = Console
  [../]
[]

[ICs]
  [./PressureIC]
    variable = gas_pressure
    type = FunctionIC
    function = 1e6*(1+x/100)
  [../]
  [./SaturationIC]
    variable = gas_saturation
    type = FunctionIC
    function = x/100
  [../]
[]

[BCs]
  [./Pleft]
    type = DirichletBC
    value = 1e6
    variable = gas_pressure
    boundary = left
  [../]
  [./Pright]
    type = DirichletBC
    value = 2e6
    variable = gas_pressure
    boundary = right
  [../]
  [./Sleft]
    type = DirichletBC
    value = 0
    variable = gas_saturation
    boundary = left
  [../]
  [./Sright]
    type = DirichletBC
    value = 1
    variable = gas_saturation
    boundary = right
  [../]
[]

[VectorPostprocessors]
  [./grad_pressure]
    type = LineValueSampler
    execute_on = 'timestep_end'
    sort_by = x
    start_point = '0 0 0'
    end_point = '100 0 0'
    num_points = 11
    variable = 'grad_liquid_pressure grad_gas_pressure liquid_phase_flux gas_phase_flux'
  [../] 
[]

