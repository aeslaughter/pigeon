[Mesh]
  type = GeneratedMesh
  dim = 1
  nx =  10
  xmax = 100
[]

[Variables]
  active = 'x2'
  [./pressure]
  [../]
  [./x2]
  [../]
[]

[AuxVariables]
  [./pressure]
  [../]
  [./saturation]
  [../]
  [./density]
  [../]
  [./viscosity]
  [../]
  [./relperm]
  [../]
  [./x1]
  [../]
  [./mobility]
  [../]
  [./ddensitydp]
  [../]
  [./ddensityds]
  [../]
  [./ddensitydx1]
  [../]
  [./ddensitydx2]
  [../]
  [./drelperm]
  [../]
  [./dmobilitydp]
  [../]
  [./dmobilityds]
  [../]
  [./dmobilitydx1]
  [../]
  [./dmobilitydx2]
  [../]
[]

[Kernels]
  active = 'Diffusion'
  [./Diffusion]
    type = Diffusion
    variable = x2
  [../]
[]

[AuxKernels]
  [./PressureAux]
    type = FluidStateTestAux
    variable = pressure
    state_property_enum = pressure
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./SaturationAux]
    type = FluidStateTestAux
    variable = saturation
    state_property_enum = saturation
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./DensityAux]
    type = FluidStateTestAux
    variable = density
    state_property_enum = density
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./ViscosityAux]
    type = FluidStateTestAux
    variable = viscosity
    state_property_enum = viscosity
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./RelpermAux]
    type = FluidStateTestAux
    variable = relperm
    state_property_enum = relperm
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./X1Aux]
    type = FluidStateTestAux
    variable = x1
    state_property_enum = mass_fraction
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 0
  [../]
  [./MobilityAux]
    type = FluidStateTestAux
    variable = mobility
    state_property_enum = mobility
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./DDensityDPAux]
    type = FluidStateTestAux
    variable = ddensitydp
    state_property_enum = ddensity_dp
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./DDensityDSAux]
    type = FluidStateTestAux
    variable = ddensityds
    state_property_enum = ddensity_ds
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./DDensityDX1Aux]
    type = FluidStateTestAux
    variable = ddensitydx1
    state_property_enum = ddensity_dx
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 0
  [../]
  [./DDensityDX2Aux]
    type = FluidStateTestAux
    variable = ddensitydx2
    state_property_enum = ddensity_dx
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 1
  [../]
  [./DRelpermAux]
    type = FluidStateTestAux
    variable = drelperm
    state_property_enum = drelperm
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./DMobilityDPAux]
    type = FluidStateTestAux
    variable = dmobilitydp
    state_property_enum = dmobility_dp
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./DMobilityDSAux]
    type = FluidStateTestAux
    variable = dmobilityds
    state_property_enum = dmobility_ds
    execute_on = 'initial'
    fluid_state_uo = FluidState
  [../]
  [./DMobilityDX1Aux]
    type = FluidStateTestAux
    variable = dmobilitydx1
    state_property_enum = dmobility_dx
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 0
  [../]
  [./DMobilityDX2Aux]
    type = FluidStateTestAux
    variable = dmobilitydx2
    state_property_enum = dmobility_dx
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 1
  [../]
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    density = 2600.
    gravity = '0 0 0'
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
    execute_on = 'initial'
    fluid_property_uo = FluidPropertiesWater
    pressure_variable = 1e6
    temperature_variable = 100
    mass_fraction_variable = x2
    num_components = 2
    component_density_increase = 200
    component_index = 1
  [../]
[]

[VectorPostprocessors]
  [./nodalproperties]
    type = NodalValueSampler
    sort_by = x
    variable = 'x2 pressure saturation density viscosity relperm x1 mobility ddensitydp ddensityds ddensitydx1 ddensitydx2 drelperm dmobilitydp dmobilityds dmobilitydx1 dmobilitydx2'
  [../]
[]

[Executioner]
  # trans_ss_check = true
  type = Steady
  solve_type = NEWTON
#  petsc_options = '-snes_check_jacobian -snes_check_jacobian_view'
[]

[Outputs]
#  output_initial = true
  file_base = singlephase2
  csv = true
[]

[ICs]
  [./Xmass]
    variable = x2
    type = FunctionIC
    function = 1-x/100
  [../]
[]

[Problem]
  solve = false
[]
