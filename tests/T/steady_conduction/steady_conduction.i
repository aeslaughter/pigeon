[Mesh]
	file = cyl-tet.e
[]

[Variables]
	active = 'temperature'
	[./temperature]
		order = FIRST
		family = LAGRANGE
	[../]
[]

[Kernels]
	active = 'thermal_conduction'
	[./thermal_conduction]
		type = ThermalConduction
		variable = temperature
	[../]
[]

[BCs]
	active = 'top_temp bottom_temp'
	[./top_temp]
		type = DirichletBC
		variable = temperature
		boundary = top
		value = 0.0
	[../]
	[./bottom_temp]
		type = DirichletBC
		variable = temperature
		boundary = bottom
		value = 1.0
	[../]
[]

[Materials]
	[./porous_material]
		type = PorousMaterial
		block = 1
		outputs = exodus

		porosity = 0.2
		permeability = 0.0001

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

	petsc_options_iname = '-pc_type -pc_hypre_type'
	petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  file_base = out
  exodus = true
[]





