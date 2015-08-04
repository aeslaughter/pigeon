[Mesh]
  type = GeneratedMesh
  dim = 2
  ny = 5
  nx = 5
  xmax = 100
  ymax = 100
[]

[Variables]
  [./x1]
  [../]
[]

[AuxVariables]
  [./density]
  [../]
  [./pressure]
    initial_condition = 1e6
  [../]
[]

[Kernels]
  active = 'ComponentAdvectiveFlux'
  [./ComponentAdvectiveFlux]
    type = ComponentAdvectiveFlux
    variable = x1
    component_index = 0
    fluid_state_uo = FluidState
    component_mass_fraction_variable = x1
  [../]
[]

[AuxKernels]
  [./DensityAux]
    type = FluidStateAux
    variable = density
    state_property_enum = density
    execute_on = 'initial'
    fluid_state_uo = FluidState
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
    diffusivity = '1e-5'
    primary_pressure_variable = pressure
    primary_saturation_variable = 1
    temperature_variable = 100
    fluid_density_variables = density
    primary_mass_fraction_variable = x1
  [../]
[]

[UserObjects]
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
    execute_on = initial
  [../]
  [./FluidState]
    type = FluidStateSinglePhase
    execute_on = 'initial'
    fluid_property_uo = FluidPropertiesWater
    pressure_variable = pressure
    temperature_variable = 100
    mass_fraction_variable = x1
    num_components = 1
    component_index = 0
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

[BCs]
  [./x1Top]
    boundary = top
    variable = x1
    type = DirichletBC
    value = 1
  [../]
  [./x1Bottom]
    boundary = bottom
    variable = x1
    type = DirichletBC
    value = 0.1
  [../]
[]
