# Test Jacobian terms
# kernels: ComponentAdvectiveFlux and ComponentMassTimeDerivative kernels
# Variable type: pressure
# FluidState: SinglePhase
# Number of components: 1
# Gravity: 0
# Mesh dimensions: 2

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  ny = 2
  xmax = 100
  ymax = 100
[]

[Variables]
  [./pressure]
  [../]
[]

[AuxVariables]
  [./density]
  [../]
  [./residual]
  [../]
[]

[Kernels]
  active = 'ComponentMassTimeDerivative ComponentAdvectiveFlux'
  [./ComponentMassTimeDerivative]
    type = ComponentMassTimeDerivative
    variable = pressure
    component_index = 0
    fluid_state_uo = FluidState
    fluid_density_variable = density
    fluid_saturation_variable = 1
    component_mass_fraction_variable = 1
    save_in = residual
  [../]
  [./ComponentAdvectiveFlux]
    type = ComponentAdvectiveFlux
    variable = pressure
    fluid_state_uo =  FluidState
    component_mass_fraction_variable = 1
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
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    density = 2600.
    gravity = '0 0 0'
    liquid_saturation_variable = 1
    permeability = '1e-10 0 0 0 1e-10 0 0 0 1e-10'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    block = 0
    fluid_state_uo = FluidState
    diffusivity = '1'
    primary_pressure_variable = pressure
    primary_saturation_variable = 1
    temperature_variable = 100
    fluid_density_variables = density
    primary_mass_fraction_variable = 1
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
    pressure_variable = pressure
    temperature_variable = 100
    mass_fraction_variable = 1
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
  active = 'smp'
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
    type = RandomIC
    variable = pressure
    max = 2e6
    min = 1e6
  [../]
[]
