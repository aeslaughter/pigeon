[GlobalParams]
  order = SECOND
  family = LAGRANGE
[]


[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
 elem_type = QUAD8
[]

[Variables]
  [./temp]
    initial_condition = 100
  [../]
[]

[Kernels]
  [./heat]
    type = HeatConduction
    variable = temp
  [../]
[]

[BCs]
  [./temp_bottom]
    type = FunctionPresetBC
    boundary = 0
    variable = temp
    function = temp
  [../]

  [./temp_top]
    type = PresetBC
    boundary = 2
    variable = temp
    value = 100
  [../]
[]

[Materials]

  [./heat1]
    type = ExtendedHeatConductionMaterial
    block = '0'
    specific_heat = 1.0
    thermal_conductivity_temperature_function = tempFnt
    temp = temp
    density_function = densityFnt
  [../]
[]

[Functions]
  [./tempFnt]
    type = ParsedFunction
    #this function actually uses temp from coupled var in material
    #t here refers to a generic function parameter: F(t) or F(batman) and NOT time
    #it might be useful later to specialize parsed function to only take one value 
    value = '2.5*t'
  [../]
  [./densityFnt]
    type = ParsedFunction
    value = 'sqrt(t)'
  [../]
  [./temp]
    type = PiecewiseLinear
    x = '0   1   2'
    y = '100 200 200'
  [../]
[]

[Executioner]
  type = Transient

  #Preconditioned JFNK (default)
  solve_type = 'PJFNK'




  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'


  line_search = 'none'


  nl_abs_tol = 9e-2
  nl_rel_tol = 1e-12

  l_tol = 1e-8
  l_max_its = 100

  start_time = 0.0
  dt = 2e-1
  end_time = 2.0

  [./Quadrature]
    order = THIRD
  [../]

[]

[Postprocessors]

  [./temp_left]
    type = SideAverageValue
    boundary = 2
    variable = temp
  [../]

  [./temp_right]
    type = SideAverageValue
    boundary = 3
    variable = temp
  [../]

  [./flux_left]
    type = SideFluxIntegral
    variable = temp
    boundary = 2
    diffusivity = thermal_conductivity
  [../]

  [./flux_right]
    type = SideFluxIntegral
    variable = temp
    boundary = 3
    diffusivity = thermal_conductivity
  [../]


[Outputs]
  [./tempProfile]
    output_material_properties = true
    type = Exodus
  [../]

  [./console]
    type = Console
    perf_log = true
    linear_residuals = true
  [../]
[]
