[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 1
  xmax = 10
  ymax = 10
[]

[Variables]
  [./liquid_pressure]
  [../]
[]

[Functions]
  [./PressureICFunction]
    type = ParsedFunction
    value = 1e6*(2-y/10)
  [../]
[]

[Kernels]
  [./ComponentMassFlux]
    type = ComponentFlux
    variable = liquid_pressure
    fluid_viscosity_variable = 0.005
    fluid_density_variable = 1000.
    primary_variable_type = pressure
    xfluid_variable = 1.0
    relperm_variable = 1.0
    fluid_pressure_variable = liquid_pressure
  [../]
[]

[BCs]
  [./PBCbottom]
    type = DirichletBC
    variable = liquid_pressure
    boundary = bottom
    value = 2e6
  [../]
  [./PBCtop]
    type = DirichletBC
    variable = liquid_pressure
    boundary = top
    value = 1e6
  [../]
[]

[Materials]
  [./PorousMaterial]
    type = PorousMaterial
    block = 0
    rel_perm_uo = RelativePermeabilityVanGenuchten
    density = 2600.
    cap_pres_uo = CapillaryPressure
    gravity = '0 9.81 0'
    liquid_saturation_variable = 1
    diffusivity = 0.
    permeability = '1e-13 0 0 0 1e-13 0 0 0 1e-13'
    porosity = 0.25
  [../]
[]

[Postprocessors]
  [./h2omass]
    type = ComponentMassPostprocessor
    fluid_density_variable = 1000.
  [../]
  [./h20fluxtop]
    type = SideMassFluxIntegralPostprocessor
    variable = liquid_pressure
    fluid_viscosity_variable = 0.005
    fluid_density_variable = 1000.
    component_variable = 1
    boundary = top
    mass_fraction_variable = 1
    relative_permeability_variable = 1
  [../]
  [./h2ofluxbottom]
    type = SideMassFluxIntegralPostprocessor
    variable = liquid_pressure
    fluid_viscosity_variable = 0.005
    fluid_density_variable = 1000.
    component_variable = 1
    boundary = bottom
    mass_fraction_variable = 1
    relative_permeability_variable = 1
  [../]
[]

[UserObjects]
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
[]

[Preconditioning]
  [./fdp]
    type = FDP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  end_time = 1
[]

[Outputs]
  csv = true
[]

[ICs]
  [./PressureIC]
    function = PressureICFunction
    variable = liquid_pressure
    type = FunctionIC
  [../]
[]

