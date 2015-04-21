[GlobalParams]
  op_num = 4 # Number of grains
  var_name_base = gr # Base name of grains
[]

[Mesh]
  file = twosquare-1.e
  dim = 2		#Problem dimensions
[]

[Variables]

  [./c]  #Concentration		if following triso model units are moles/m^3
    block = '1 2'
  [../]

  [./temp]
#    initial_condition = 500
#    scaling = 1e18
  [../]

  [./PolycrystalVariables]
  [../]

[]

[AuxVariables]

  [./bnds]		# Variable used to visualize the grain boundaries in the simulation
    order = FIRST
    family = LAGRANGE
  [../]

  [./grain_radius]
    block = '1 2'
    initial_condition = 10e-6
  [../]
[]

[Functions]

  [./temp_bc]
    type = PiecewiseLinear
    x = '0    31.5e8'
    y = '1500 1200'
  [../]

[]

[Kernels]
#  [./soret]
#   type = ThermoDiffusion
#    variable = c
#    temp = temp
#  [../]

#  [./diff]
#    type = CalcDiffusion
#    variable = c
#    coef = 0.1
#    block = '1 2'
#  [../]

  [./td]
    type = TimeDerivative
    variable = c
  [../]

  [./heat_ie]
    type = HeatConductionTimeDerivative
    variable = temp
  [../]
 

 [./heat]
    type = HeatConduction
    variable = temp
 [../]

  [./mass_ie]
    type = TimeDerivative
    variable = c
  [../]

  [./mass]
    type = ArrheniusDiffusion
    variable = c
    block = '1 2'
  [../] 

  [./mass_source]
    type = BodyForce
    variable = c
#    function = power_history
#    value = 1.22e-5     # units of moles/m**3-s
    block = 1
  [../]
 
  [./mass_decay]
    type = Decay
    variable = c
  [../]

  [./PolycrystalKernel]		
  [../]

[]

[AuxKernels]
  # AuxKernel block, defining the equations used to calculate the auxvars
  [./bnds_aux]
    # AuxKernel that calculates the GB term
    type = BndsCalcAux
    variable = bnds
    execute_on = timestep_end
  [../]
 
  [./grain_radius]
    type = GrainRadiusAux
    block = '1 2'
    variable = grain_radius
    temp = temp
  [../]

[]

[ICs]

  [./Left_Box]
    type = BoundingBoxIC
    variable = c
    x1 = 0.0
    y1 = 0.0
    x2 = 1.0
    y2 = 1.0
    inside = 5			#value of the variable inside the box
    outside = 1			#value of the variable outside the box	       
  [../]

#  [./Right_Box]
#    type = BoundingBoxIC
#    variable = c
#    x1 = 1.0
#    y1 = 0.0
#    x2 = 2.0
#    y2 = 1.0
#    inside = 1			#value of the variable inside the box
#    outside = 5			#value of the variable outside the box	       
#  [../]

  [./PolycrystalICs]
    [./PolycrystalVoronoiIC]
      grain_num = 15
    [../]
  [../]
[]



[BCs]
 
   [./temp]
    type = FunctionDirichletBC
    variable = temp
    boundary = '1 4'
    function = temp_bc
  [../]

  [./Periodic]
    [./top_bottom]
      primary = '5 6'
      secondary = '2 3'
#      translation = '0 1 0' 
    [../]
 
    [./left_right]
      primary = 1
      secondary = 4
#      translation = '2 0 0'
    [../]
  [../]

[]

[Materials]

#  [./badm]
#    type = BlockAverageDiffusionMaterial
#    block = '1 2'
#    block_average_userobject = bav
#  [../]


  [./right_material]				#rename to selected material type
    type = ArrheniusDiffusionCoef
    block ='1 2'
    d1 = 3.6e-4			# m^2/s
    q1 = 189e3			# J/mol
    d2 = 0			# m^2/s
    q2 = 0			# J/mol
    gas_constant = 8.3143	# J/K-mol
    temp = temp
#    arrhenius_diffusion_coef = arrhenius_diffusion_coef
  [../]

#  [./left_material]
#    type = ArrheniusDiffusionCoef
#    block = 1
#    d1 = 1.0e-8			# m^2/s
#    q1 = 261.0e3
#    d2 = 0.0
#    q2 = 0.0
#    gas_constant = 8.3143     # J/K-mol
#    temp = temp
#  [../]

  [./material_temp]
     type = HeatConductionMaterial
     block = '1 2'
     thermal_conductivity = 4.0
     temp = temp
  [../]

  [./material]
    type = GenericConstantMaterial
    block = '1 2'
    prop_names = 'mass_diffusivity heat_of_transport diffusion_coef density'
    prop_values = ' 1 1 0.1 11000.0'
  [../]

  [./radio_active_decay_constant]
    type = RadioActiveDecayConstant
    block = '1 2'
    lambda = 7.297e-10	 # units:(1/sec)  The constant for Cesium
  [../]

  [./CuGrGr]
    type = GBEvolution # Quantitative material properties for copper grain growth.	 Dimensions are nm and ns
    block = '1 2' # Block ID 
    GBmob0 = 2.5e-6 # Mobility prefactor for Cu from Schonfelder1997
    GBenergy = 0.708 # GB energy for Cu from Schonfelder1997
    Q = 0.23 # Activation energy for grain growth from Schonfelder 1997
    T = 500 # Constant temperature of the simulation (for mobility calculation)
    wGB = .014 # Width of the diffuse GB in nm
    length_scale = 1e-6
  [../]



[]

[UserObjects]
#  [./bav]
#   type = BlockAverageValue
#   variable = c
#   execute_on = timestep_begin
#  [../]
[]


#[Executioner]
#  type = Transient
#  scheme = bdf2
# 
#  solve_type ='PJFNK'
#  petsc_options_iname = ' -pc_tyoe -pc_hypre_type -ksp_gmres_restart'
#  petsc_options_value = 'hypre boomeramg 31'
#  l_tol = 1.0e-4
#  nl_max_its = 1e-11
#  nl_rel_tol = 1.0e-4
#  start_time = -300.0 
#  end_time = 31.5e5
#  #num_steps = 2
#  dt = 5e3
# 
#  [./Adaptivity]
#    initial_adaptivity = 2
#    refine_fraction = 0.7
#    coursen_fraction = 0.1
#   max_h_level = 4 
#  [../]
#[]

[Executioner]
  type = Transient
  start_time = -1000.0
  n_startup_steps = 5
  end_time = 31.5e8
  #num_steps = 2500
  dt = 5e5
  
 
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201		    hypre    boomeramg	    4'
 
  line_search = 'none'
 
  #Preconditioned JFNK (default)
  solve_type = 'PJFNK'
 
  nl_rel_tol = 5e-4
  nl_abs_tol = 1e-10
  nl_max_its = 15
 
  l_tol = 1e-3
  l_max_its = 50
 
[]

#[Executioner]
#  type = Transient
#  scheme = bdf2
#  solve_type = PJFNK
#  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart -mat_mffd_type'
#  petsc_options_value = 'hypre boomeramg 101 ds'
#  l_max_it = 30
#  l_tol = 1e-5
#  nl_max_its = 40
#  nl_abs_tol = 1e-9
#  nl_rel_tol = 1e-9
#  start_time = -300.0
#  end_time = 31.5e5
#  [./TimeStepper]
#    type = IterationAdaptiveDT
#    dt = 500
#    optimal_iterations = 8
##    growth_factor = 1.25
#  [../]
#  [./Adaptivity]
#    initial_adaptivity = 2
#    refine_fraction = 0.7
#    coarsen_fraction = 0.1
#    max_h_level = 4
#  [../]
#[]


[Postprocessors]
  
  [./left_average_conc]
    type = ElementAverageValue
    block = 1
    variable = c
  [../]
 
  [./right_average_conc]
    type = ElementAverageValue
    block = 2
    variable = c
  [../]
 
  [./temp]
    type =ElementAverageValue
    variable = temp
  [../]
  
  [./right_diffusion_coef]
    type = ElementAverageValue
    block = 2
    variable = arrhenius_diffusion_coef
  [../]

  [./left_diffusion_coef]
    type = ElementAverageValue
    block = 1
    variable = arrhenius_diffusion_coef
  [../]

  [./left_grain_radius]
    type = ElementAverageValue
    block = 1
    variable = grain_radius
  [../]
 
  [./right_grain_radius]
    type = ElementAverageValue
    block = 2
    variable = grain_radius
  [../]

 
  [./num_grains]
    type = NodalFloodCount
    variable = bnds
    threshold = 0.7
  [../]

  [./gr1area]
    type = ElementIntegralVariablePostprocessor
    variable = gr1
    execute_on = 'initial timestep_end'
  [../]

[]


[Outputs]
#  exodus = true
  csv = true
  output_on = 'initial timestep_end'

  [./console]
    type = Console
    perf_log = true
    output_on = 'timestep_end failed nonlinear linear'
#    output_screen = 0
  [../]

  [./exodus]
    type = Exodus
    show_material_properties = arrhenius_diffusion_coef
    output_material_properties = 1

[]
