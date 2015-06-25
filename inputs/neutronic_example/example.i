[mesh]
  type = GeneratedMesh dim = 2
  xmin = 0
  xmax = 10
  ymin = 0
  ymax = 10
  elem_type = QUAD4
  nx = 100
  ny = 100
[]

[Variables] 
  active = ’flux’ 
  [./flux]
    order = FIRST
    family = LAGRANGE 
  [../]
[]

[Materials]
  active = ’SingleFissionMaterial’
  [./SingleFissionMaterial]
    type = DiffusionNeutronicsMaterial block = 0
    diffusion_coef = 2.0
    sigma_abs = 1.0
    nusigma_xs = 1.1
  [../]
[]

[Kernels]
  active = ’diffusion removal source’ 
  [./diffusion]
    type = NeutronicDiffusion
    variable = flux 
  [../]
  [./removal]
    type = NeutronicRemoval 
    variable = flux
  [../]

  [./source]
    type = NeutronicSource 
    variable = flux
  [../] 
[]

[BCs]
  active = ’leakage’ 
  [./leakage]
    type = DirichletBC 
    variable = flux 
    boundary = ’0 1’ 
    value = 0
  [../] 
  [./reflective]
    type = NeumannBC 
    variable = flux 
    boundary = ’2 3’ 
    value = 0
  [../] 
[]

[Executioner]
  type = NonlinearEigen
  
  bx_norm = ’unorm’ 
  normalization = ’unorm’ 
  normal_factor = 1.0

  free_power_iterations = 8 
  source_abs_tol = 1e-12 
  source_rel_tol = 1e-50
  k0 = 1.0

  #Preconditioned JFNK (default)
  solve_type = ’PJFNK’ 
[]

[Postprocessors] 
  active = ’unorm’ 
  [./unorm]
    type = ElementIntegralVariablePostprocessor 
    variable = flux
    # execute on residual is
    # important for nonlinear
    # eigen solver!
    execute_on = linear 
  [../]
[]

[Outputs]
  file_base = Mosquito 
  interval = 1
  exodus = true 
  [./console]
    type = Console
    perf_log = true
    output_on = ’initial failed nonlinear linear timestep_end’
  [../] 
[]
