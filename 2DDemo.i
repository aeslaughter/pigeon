[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 50
  ny = 50
  xmin = -0.5
  xmax = 0.5
  ymin = -0.5
  ymax = 0.5
  elem_type = TRI3
[]

[Variables]
  [./u]
    [./InitialCondition]
      variable = u
      type = ConstantIC
      value = 295
    [../]
  [../]
[]

[Kernels]
  [./Time]
    type = TimeDerivative
    variable = u
  [../]
  [./Diffusion]
    type = HeatDiff
    variable = u
  [../]
  [./Air]
    type = Conv2D
    variable = u
    mu = 0.1
    u_e = 295
  [../]
  [./Laser]
    type = LaserSource
    variable = u
    sigma = 0.01
    power = 1000
    y0 = 0.1*sin(2.0*t)
    x0 = 0.1*cos(2.0*t)
  [../]
[]

[BCs]
  [./Air]
    type = ConvectBC
    variable = u
    boundary = 'bottom left right top'
    u_e = 295
    mu = 0.1
  [../]
[]

[Materials]
  [./Aluminium]
    type = Plate
    block = 0
    c_p = 910
    rho = 2700
    alpha = 0.00008418
    l = 0.01
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  start_time = 0
  end_time = 6.2831
  dt = 0.0174
[]

[Outputs]
  output_initial = true
  exodus = true
  output_final = true
  file_base = 2DDemo
  print_perf_log = true
[]

