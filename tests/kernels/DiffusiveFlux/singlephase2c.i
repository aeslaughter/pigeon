[Mesh]
  type = GeneratedMesh
  dim = 2
  ny = 3
  nx =  3
  xmax = 100
  ymax = 100
[]

[Variables]
  [./x2]
  [../]
[]

[AuxVariables]
  [./density]
  [../]
  [./x1]
  [../]
[]

[Kernels]
  [./Component2DiffusiveFlux]
    type = ComponentDiffusiveFlux
    variable = x2
    component_index = 1
    fluid_state_uo = FluidState
    fluid_density_variable = density
    component_mass_fraction_variable = x2
  [../]
[]

[AuxKernels]
  [./DensityAux]
    type = FluidStateAux
    variable = density
    state_property_enum = density
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
  [../]
  [./X1Aux]
    type = FluidStateAux
    variable = x1
    state_property_enum = mass_fraction
    execute_on = 'initial linear'
    fluid_state_uo = FluidState
    component_index = 0
  [../]
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    density = 2600.
    gravity = '0 -10 0'
    liquid_saturation_variable = 1
    permeability = '1e-12 0 0 0 1e-12 0 0 0 1e-12'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    block = 0
    fluid_state_uo = FluidState
    diffusivity = '1e-5 1e-5'
    primary_pressure_variable = 1e6
    primary_saturation_variable = 1
    temperature_variable = 100
    fluid_density_variables = density
    primary_mass_fraction_variable = x2
  [../]
[]

[UserObjects]
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
    execute_on = initial
  [../]
  [./FluidState]
    type = FluidStateSinglePhase
    execute_on = 'initial linear'
    fluid_property_uo = FluidPropertiesWater
    pressure_variable = 1e6
    temperature_variable = 100
    mass_fraction_variable = x2
    num_components = 2
    component_density_increase = 200
    component_index = 1
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  end_time = 1
  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-8
  l_tol = 1e-12
[]

[Outputs]
  output_initial = true
  print_linear_residuals = true
  print_perf_log = true
[]

[ICs]
  [./Xmass]
    variable = x2
    type = FunctionIC
    function = 0.044*(1-x/100)
  [../]
[]

[BCs]
  [./topX]
    type = PresetBC
    variable = x2
    boundary = left
    value = 0.044
  [../]
  [./bottomX]
    type = DirichletBC
    variable = x2
    boundary = right
    value = 0
  [../]
[]
