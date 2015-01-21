[Mesh]
	file = cyl-tet.e
[]

[Variables]
	active = 'pressure'
	[./pressure]
		order = FIRST
		family = LAGRANGE
	[../]
[]

[Kernels]
	active = 'pressure_darcy'
	[./pressure_darcy]
		type = PressureDarcy
		variable = pressure
	[../]
[]

[BCs]
	active = 'top_press bottom_press'
	[./top_press]
		type = DirichletBC
		variable = pressure
		boundary = top
		value = 1.0e5
	[../]
	[./bottom_press]
		type = DirichletBC
		variable = pressure
		boundary = bottom
		value = 2.0e5
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





