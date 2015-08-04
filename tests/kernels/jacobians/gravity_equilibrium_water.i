[Mesh]
  type = GeneratedMesh
  dim = 2
  ny = 2
  xmax = 100
  ymax = 100
[]

[Variables]
  active = 'liquid_pressure'
  [./liquid_saturation]
  [../]
  [./liquid_pressure]
  [../]
[]

[AuxVariables]
  [./liquid_density]
  [../]
  [./liquid_viscosity]
  [../]
  [./liquid_relperm]
  [../]
[]

[Functions]
  [./PressureICFunction]
    type = ParsedFunction
    value = 1e6
  [../]
[]

[Kernels]
  active = 'H2OMassTime'
  [./H2OMassTime]
    type = ComponentMassTimeDerivative
    variable = liquid_pressure
    liquid_density_variable = liquid_density
    xliquid_variable = 1
    gas_saturation_variable = 0
    primary_variable_type = pressure
    gas_density_variable = 0
    liquid_saturation_variable = 1
    xgas_variable = 0
  [../]
  [./H2OGasFlux]
    type = ComponentFlux
    variable = gas_pressure
    fluid_viscosity_variable = gas_viscosity
    fluid_density_variable = gas_density
    primary_variable_type = pressure
    xfluid_variable = 0
    relperm_variable = gas_relperm
    fluid_pressure_variable = gas_pressure
  [../]
  [./H2OLiquidFlux]
    type = ComponentFlux
    variable = liquid_pressure
    fluid_viscosity_variable = liquid_viscosity
    fluid_density_variable = liquid_density
    primary_variable_type = pressure
    xfluid_variable = 1.0
    relperm_variable = liquid_relperm
    fluid_pressure_variable = liquid_pressure
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
  [../]
[]

[BCs]
  active = 'PBCtop'
  [./PBCright]
    type = DirichletBC
    variable = liquid_pressure
    boundary = right
    value = 1e6
  [../]
  [./PBCtop]
    type = DirichletBC
    variable = liquid_pressure
    boundary = top
    value = 1e6
  [../]
[]

[Materials]
  active = 'PorousMaterial'
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    rel_perm_uo = RelativePermeabilityVanGenuchten
    density = 2600.
    cap_pres_uo = CapillaryPressure
    gravity = '0 9.8 0'
    liquid_saturation_variable = 1
    diffusivity = 0.
    permeability = '1e-13 0 0 0 1e-13 0 0 0 1e-13'
    porosity = 0.25
  [../]
  [./FluidStateMaterial]
    type = FluidStateMaterial
    liquid_saturation_variable = liquid_saturation
    temperature_variable = temperature
    gas_pressure_variable = gas_pressure
    liquid_pressure_variable = liquid_pressure
    fluid_state_uo = FluidState
  [../]
[]

[Postprocessors]
  [./h2omass]
    type = ComponentMassPostprocessor
    fluid_density_variable = liquid_density
  [../]
  [./h2ofluxleft]
    type = SideMassFluxIntegralPostprocessor
    variable = liquid_pressure
    fluid_viscosity_variable = liquid_viscosity
    fluid_density_variable = liquid_density
    component_variable = 1
    boundary = left
  [../]
  [./h20fluxtop]
    type = SideMassFluxIntegralPostprocessor
    variable = liquid_pressure
    fluid_viscosity_variable = liquid_viscosity
    fluid_density_variable = liquid_density
    component_variable = 1
    boundary = top
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
    p0 = 1e3
    cp_max = 1e4
    m = 0.5
    sat_ls = 1.0
    sat_lr = 0.2
    execute_on = 'initial linear'
  [../]
  [./RelativePermeabilityVanGenuchten]
    type = RelativePermeabilityVanGenuchten
    sat_gr = 0.2
    m = 0.5
    sat_ls = 1.0
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

#[Preconditioning]
#  active = ''
#  [./fdp]
#    type = FDP
#    full = true
#  [../]
#  [./smp]
#    type = SMP
#  [../]
#[]

[Problem]
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
    growth_factor = 1.1
  [../]
[]

[Outputs]
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
  active = 'PressureIC'
  [./LiquidSaturationIC]
    variable = liquid_saturation
    type = ConstantIC
    value = 0.7
  [../]
  [./PressureIC]
    function = PressureICFunction
    variable = liquid_pressure
    type = FunctionIC
  [../]
[]

