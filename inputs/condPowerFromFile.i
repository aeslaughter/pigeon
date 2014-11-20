[Mesh]
  type = FileMesh
  file = cylWithBC.e
  distribution = SERIAL
[]

[AuxVariables]
  [./elementalTemp]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT    
  [../]
  # variable that will come from mesh file
  [./auxVar_power]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_volPower]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_flux1g]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_flux1g_err]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_flux2g]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_flux2g_err]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_flux3g]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_flux3g_err]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_flux4g]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  [./auxVar_flux4g_err]
    block = '1 2 3'
    family = MONOMIAL
    order = CONSTANT
  [../]
  
  # [./auxVar_keff]
  #   block = '1 2 3'
  #   family = MONOMIAL
  # [../]
  #
  # [./auxVar_stdkeff]
  #   block = '1 2 3'
  #   family = MONOMIAL
  # [../]
[]

[AuxKernels]
  [./ProjectTempOnAux]
    type = ProjectVarAux
    block = '1 2 3'
    execute_on = timestep
    variable = elementalTemp
    coupled = T
  [../]
  
  # kernel to get sol from mesh by applying a userobject to it
  [./auxKernel_power]
    type = SolutionAux
    variable = auxVar_power
    block = '1 2 3'
    solution = powerUO
    execute_on = timestep_begin
  [../]
  
  [./auxKernel_volPower]
    type = SolutionAux
    variable = auxVar_volPower
    block = '1 2 3'
    solution = volPowerUO
    execute_on = timestep
  [../]
  
  [./auxKernel_flux1g]
    type = SolutionAux
    variable = auxVar_flux1g
    block = '1 2 3'
    solution = flux1gUO
    execute_on = timestep
  [../]
  
  [./auxKernel_flux1g_err]
    type = SolutionAux
    variable = auxVar_flux1g_err
    block = '1 2 3'
    solution = flux1g_errUO
    execute_on = timestep
  [../]
  
  [./auxKernel_flux2g]
    type = SolutionAux
    variable = auxVar_flux2g
    block = '1 2 3'
    solution = flux2gUO
    execute_on = timestep
  [../]
  
  [./auxKernel_flux2g_err]
    type = SolutionAux
    variable = auxVar_flux2g_err
    block = '1 2 3'
    solution = flux2g_errUO
    execute_on = timestep
  [../]
  
  [./auxKernel_flux3g]
    type = SolutionAux
    variable = auxVar_flux3g
    block = '1 2 3'
    solution = flux3gUO
    execute_on = timestep
  [../]
  
  [./auxKernel_flux3g_err]
    type = SolutionAux
    variable = auxVar_flux3g_err
    block = '1 2 3'
    solution = flux3g_errUO
    execute_on = timestep
  [../]
  
  [./auxKernel_flux4g]
    type = SolutionAux
    variable = auxVar_flux4g
    block = '1 2 3'
    solution = flux4gUO
    execute_on = timestep
  [../]
  
  [./auxKernel_flux4g_err]
    type = SolutionAux
    variable = auxVar_flux4g_err
    block = '1 2 3'
    solution = flux4g_errUO
    execute_on = timestep
  [../]
  
  # [./auxKernel_keff]
  #   type = SolutionAux
  #   variable = auxVar_keff
  #   block = '1 2 3'
  #   solution = keffUO
  #   execute_on = timestep
  # [../]
  #
  # [./auxKernel_stdkeff]
  #   type = SolutionAux
  #   variable = auxVar_stdkeff
  #   block = '1 2 3'
  #   solution = stdkeffUO
  #   execute_on = timestep
  # [../]
[]

[Variables]
  [./T]
    block = '1 2 3'
  [../]
[]

[Kernels]
  active = 'conduction fissionPower'
  [./conduction]
    type = HeatConduction
    variable = T
    block = '1 2 3'
  [../]
  [./fissionPower]
    type = CoupledHeatSource
    variable = T
    block = '1 2 3'
    scaledPower = 3E11
    powerDensity = auxVar_power
  [../]
[]

[BCs]
  [./tOuterWall]
    type = DirichletBC
    variable = T
    boundary = '10'
    value = 100
  [../]
  
  [./tTopBottom]
    type = DirichletBC
    variable = T
    boundary = '20 21'  
    value = 200
  [../]
[]

[Materials]
  [./fuel]
    type = ExtendedHeatConductionMaterial
    block = 2
    thermal_conductivity = 14
    specific_heat = 1
    density_function = fuelDensity
    temp = T
  [../]
  [./innerAnnulus]
    type = ExtendedHeatConductionMaterial
    block = 1
    specific_heat = 1
    thermal_conductivity = 1
    density_function = middleDensity   
    temp = T    
  [../]
  [./outerAnnulus]
    type = ExtendedHeatConductionMaterial
    block = 3
    specific_heat = 1
    thermal_conductivity = 4
    density_function = outerDensityFnt  
    temp = T  
  [../]
[]

[Functions]
  [./fuelDensity]
    type = ParsedFunction
    value = '0.5*t'
  [../]
  
  [./middleDensity]
    type = ParsedFunction
    value = '0.5*t'
  [../]
  
  [./outerDensityFnt]
    type = ParsedFunction
    value = '0.1*t'
  [../]
[]

[UserObjects]
  # interface to grab elemental var from mesh (and possibly project it to different mesh)
  [./powerUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'power'
    timestep = 1
  [../]
  #Need to copy all values from output_cyl.e mesh so they can be written to later    
  [./volPowerUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'volPower'
    timestep = 1
  [../]
  
  [./flux1gUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'flux1g'
    timestep = 1
  [../]
  
  [./flux1g_errUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'flux1g_err'
    timestep = 1
  [../]
  
  [./flux2gUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'flux2g'
    timestep = 1
  [../]
  
  [./flux2g_errUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'flux2g_err'
    timestep = 1
  [../]
  
  [./flux3gUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'flux3g'
    timestep = 1
  [../]
  
  [./flux3g_errUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'flux3g_err'
    timestep = 1
  [../]
  
  [./flux4gUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'flux4g'
    timestep = 1
  [../]
  
  [./flux4g_errUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'flux4g_err'
    timestep = 1
  [../]
  
  # [./keffUO]
  #   type = SolutionUserObject
  #   mesh = output_cyl.e
  #   system_variables = 'keff'
  #   timestep = 1
  # [../]
  #
  # [./stdkeffUO]
  #   type = SolutionUserObject
  #   mesh = output_cyl.e
  #   system_variables = 'stdkeff'
  #   timestep = 1
  # [../]
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  [./tempProfile]
    output_material_properties = true
    output_nodal_variables = 0
    type = Exodus
  [../]
[]

