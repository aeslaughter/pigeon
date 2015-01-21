[Mesh]
	file = cyl-tet.e
[]

[Variables]
	active = 'temperature pressure'
	[./temperature]
		order = FIRST
		family = LAGRANGE
	[../]
	[./pressure]
		order = FIRST
		family = LAGRANGE
	[../]
[]

[Kernels]
	active = 'thermal_conduction thermal_advection pressure_darcy'
	[./thermal_conduction]
		type = ThermalConduction
		variable = temperature
	[../]
	[./thermal_advection]
		type = ThermalAdvection
		variable = temperature
		pressure_variable = pressure
	[../]
	[./pressure_darcy]
		type = PressureDarcy
		variable = pressure
	[../]
[]

[BCs]
	active = 'top_temp bottom_temp top_press bottom_press'
	[./top_temp]
		type = DirichletBC
		variable = temperature
		boundary = 'top'
		value = 0.0
	[../]
	[./bottom_temp]
		type = DirichletBC
		variable = temperature
		boundary = 'bottom'
		value = 1.0
	[../]
	[./top_press]
		type = DirichletBC
		variable = pressure
		boundary = 'top'
		value = 0.0
	[../]
	[./bottom_press]
		type = DirichletBC
		variable = pressure
		boundary = 'bottom'
		value = 1.0
	[../]
[]

[Materials]
	[./porous_material]
		type = PorousMaterial
		block = 1
		outputs = exodus

		pressure_for_Darcy = pressure
		porosity = 1.0
		permeability = 1.0

		fluid_density = 1.0
		fluid_heat_capacity = 1.0
		fluid_thermal_conductivity = 1.0
		fluid_compressibility = 1.0
		fluid_viscosity = 1.0

		solid_density = 1.0
		solid_heat_capacity = 1.0
		solid_thermal_conductivity = 1.0
	[../]
[]

[Executioner]
	type = Steady

	solve_type = 'PJFNK'
[]

[Outputs]
  output_initial = true
  file_base = out
  exodus = true
  [./console]
    type = Console
    perf_log = true
    output_on = 'timestep_end failed nonlinear linear'
  [../]
[]





