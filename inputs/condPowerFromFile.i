[Mesh]
  type = FileMesh
  file = output_cyl.e
  distribution = SERIAL
[]

[MeshModifiers]
  [./outside1]
    type = SideSetsAroundSubdomain
    block = 1
    boundary = 1
  [../]
  [./outside2]
    type = SideSetsAroundSubdomain
    block = 2
    boundary = 2
  [../]
  [./outside3]
    type = SideSetsAroundSubdomain
    block = 3
    boundary = 3
  [../]
[]

[Variables]
  [./T]
    block = '1 2 3'
  [../]
[]

[AuxVariables]
  # variable that will come from mesh file
  [./powerDensity]
    block = '1 2 3'
  [../]
[]

[Kernels]
  [./conduction]
    type = HeatConduction
    variable = T
    block = '1 2 3'
  [../]
  [./fissionPower]
    type = CoupledHeatSource
    variable = T
    block = '1 2 3'
    scaledPower = 1E5
    powerDensity = powerDensity
  [../]
[]

[AuxKernels]
  # kernel to get sol from mesh by applying a userobject to it
  [./powerDensityAux]
    type = SolutionAux
    variable = powerDensity
    block = '1 2 3'
    solution = powerDensityUO
  [../]
[]

[BCs]
  [./tOuterWall]
    type = DirichletBC
    variable = T
    boundary = '1 2 3'
    value = 100
  [../]
[]

[Materials]
  [./fuel]
    type = HeatConductionMaterial
    block = 2
    temp = T
    thermal_conductivity = 14
    specific_heat = 1
  [../]
  [./innerAnnulus]
    type = HeatConductionMaterial
    block = 1
    specific_heat = 1
    thermal_conductivity = 1
  [../]
  [./outerAnnulus]
    type = HeatConductionMaterial
    block = 3
    specific_heat = 1
    thermal_conductivity = 4
  [../]
[]

[UserObjects]
  # interface to grab elemental var from mesh (and possibly project it to different mesh)
  [./powerDensityUO]
    type = SolutionUserObject
    mesh = output_cyl.e
    system_variables = 'volPower'
    timestep = -1
  [../]
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
    type = Exodus
  [../]
[]

