[Mesh]
  type = GeneratedMesh
  dim = 1
  nx =  10
  xmax = 100
[]

[Variables]
  [./liquid_pressure]
  [../]
  [./liquid_saturation]
  [../]
[]

[AuxVariables]
  [./gas_pressure]
  [../]
  [./gas_saturation]
  [../]
  [./gas_density]
  [../]
  [./gas_viscosity]
  [../]
  [./gas_relperm]
  [../]
  [./x1]
  [../]
  [./gasmobility]
  [../]
  [./dgasdensitydp]
  [../]
  [./dgasdensityds]
  [../]
  [./dgasdensitydx1]
  [../]
  [./dgasrelperm]
  [../]
  [./dgasmobilitydp]
  [../]
  [./dgasmobilityds]
  [../]
  [./dgasmobilitydx1]
  [../]
  [./liquid_density]
  [../]
  [./liquid_viscosity]
  [../]
  [./liquid_relperm]
  [../]
  [./liquid_mobility]
  [../]
  [./dliquiddensitydp]
  [../]
  [./dliquiddensityds]
  [../]
  [./dliquiddensitydx1]
  [../]
  [./dliquidrelperm]
  [../]
  [./dliquidmobilitydp]
  [../]
  [./dliquidmobilityds]
  [../]
  [./dliquidmobilitydx1]
  [../]
[]

[Kernels]
  active = 'Diffusion'
  [./Diffusion]
    type = Diffusion
    variable = liquid_pressure
  [../]
[]

[AuxKernels]
  [./GasPressureAux]
    type = FluidStateTestAux
    variable = gas_pressure
    state_property_enum = pressure
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasSaturationAux]
    type = FluidStateTestAux
    variable = gas_saturation
    state_property_enum = saturation
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasDensityAux]
    type = FluidStateTestAux
    variable = gas_density
    state_property_enum = density
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasViscosityAux]
    type = FluidStateTestAux
    variable = gas_viscosity
    state_property_enum = viscosity
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasRelpermAux]
    type = FluidStateTestAux
    variable = gas_relperm
    state_property_enum = relperm
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./X1Aux]
    type = FluidStateTestAux
    variable = x1
    state_property_enum = mass_fraction
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 0
  [../]
  [./GasMobilityAux]
    type = FluidStateTestAux
    variable = gasmobility
    state_property_enum = mobility
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasDDensityDPAux]
    type = FluidStateTestAux
    variable = dgasdensitydp
    state_property_enum = ddensity_dp
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasDDensityDSAux]
    type = FluidStateTestAux
    variable = dgasdensityds
    state_property_enum = ddensity_ds
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasDDensityDX1Aux]
    type = FluidStateTestAux
    variable = dgasdensitydx1
    state_property_enum = ddensity_dx
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 0
    phase_index = 1
  [../]
  [./GasDRelpermAux]
    type = FluidStateTestAux
    variable = dgasrelperm
    state_property_enum = drelperm
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasDMobilityDPAux]
    type = FluidStateTestAux
    variable = dgasmobilitydp
    state_property_enum = dmobility_dp
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasDMobilityDSAux]
    type = FluidStateTestAux
    variable = dgasmobilityds
    state_property_enum = dmobility_ds
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 1
  [../]
  [./GasDMobilityDX1Aux]
    type = FluidStateTestAux
    variable = dgasmobilitydx1
    state_property_enum = dmobility_dx
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 0
    phase_index = 1
  [../]
  [./LiquidDensityAux]
    type = FluidStateTestAux
    variable = liquid_density
    state_property_enum = density
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidViscosityAux]
    type = FluidStateTestAux
    variable = liquid_viscosity
    state_property_enum = viscosity
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidRelpermAux]
    type = FluidStateTestAux
    variable = liquid_relperm
    state_property_enum = relperm
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidMobilityAux]
    type = FluidStateTestAux
    variable = liquid_mobility
    state_property_enum = mobility
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidDDensityDPAux]
    type = FluidStateTestAux
    variable = dliquiddensitydp
    state_property_enum = ddensity_dp
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidDDensityDSAux]
    type = FluidStateTestAux
    variable = dliquiddensityds
    state_property_enum = ddensity_ds
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidDDensityDX1Aux]
    type = FluidStateTestAux
    variable = dliquiddensitydx1
    state_property_enum = ddensity_dx
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 0
    phase_index = 0
  [../]
  [./LiquidDRelpermAux]
    type = FluidStateTestAux
    variable = dliquidrelperm
    state_property_enum = drelperm
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidDMobilityDPAux]
    type = FluidStateTestAux
    variable = dliquidmobilitydp
    state_property_enum = dmobility_dp
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidDMobilityDSAux]
    type = FluidStateTestAux
    variable = dliquidmobilityds
    state_property_enum = dmobility_ds
    execute_on = 'initial'
    fluid_state_uo = FluidState
    phase_index = 0
  [../]
  [./LiquidDMobilityDX1Aux]
    type = FluidStateTestAux
    variable = dliquidmobilitydx1
    state_property_enum = dmobility_dx
    execute_on = 'initial'
    fluid_state_uo = FluidState
    component_index = 0
    phase_index = 0
  [../]
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    density = 2600.
    gravity = '0 0 0'
    liquid_saturation_variable = liquid_saturation
    permeability = '1e-12 0 0 0 1e-12 0 0 0 1e-12'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    block = 0
    fluid_state_uo = FluidState
    diffusivity = '1e-5'
    primary_pressure_variable = liquid_pressure
    primary_saturation_variable = liquid_saturation
    temperature_variable = 100
    fluid_density_variables = 'liquid_density gas_density'
    primary_mass_fraction_variable = x1
  [../]
[]

[UserObjects]
  [./FluidPropertiesWater]
    type = FluidPropertiesWater
    execute_on = initial
  [../]
  [./FluidPropertiesCO2]
    type = FluidPropertiesCO2
  [../]
  [./RelativePermeability]
    type = RelativePermeabilityVanGenuchten
    m = 0.5
    sat_gr = 0
    sat_lr = 0
    sat_ls = 1
  [../]
  [./CapillaryPressure]
    type = CapillaryPressureVanGenuchten
    m = 0.5
    sat_lr = 0
    sat_ls = 1
    cp_max = 1e5
    p0 = 1e4
  [../]
  [./FluidState]
    type = FluidStateTwoPhase
    execute_on = 'initial'
    liquid_property_uo = FluidPropertiesWater
    gas_property_uo = FluidPropertiesCO2
    pressure_variable = liquid_pressure
    temperature_variable = 100
    mass_fraction_variable = 1
    saturation_variable = liquid_saturation
    saturation_variable_phase = 0
    pressure_variable_phase = 0
    num_components = 1
    component_index = 0
    capillary_pressure_uo = CapillaryPressure
    relative_permeability_uo = RelativePermeability
  [../]
[]

[VectorPostprocessors]
  [./nodalproperties]
    type = NodalValueSampler
    sort_by = x
    variable = 'gas_pressure gas_saturation gas_density gas_viscosity gas_relperm x1 gasmobility dgasdensitydp dgasdensityds dgasdensitydx1 dgasrelperm dgasmobilitydp dgasmobilityds dgasmobilitydx1 liquid_pressure liquid_saturation liquid_density liquid_viscosity liquid_relperm liquid_mobility dliquiddensitydp dliquiddensityds dliquiddensitydx1 dliquidrelperm dliquidmobilitydp dliquidmobilityds dliquidmobilitydx1'
  [../]
[]

[Executioner]
  # trans_ss_check = true
  type = Steady
  solve_type = NEWTON
#  petsc_options = '-snes_check_jacobian -snes_check_jacobian_view'
[]

[Outputs]
  file_base = twophase1
  csv = true
[]

[ICs]
  [./Pressure]
    variable = liquid_pressure
    type = FunctionIC
    function = (2-x/100)*1e6
  [../]
  [./Saturation]
    variable = liquid_saturation
    type = ConstantIC
    value = 0.7
  [../]
[]

[Problem]
  solve = false
[]
