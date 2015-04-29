# #############################################################################
# Test the CapillaryPressureConstant UserObject. This capillary pressure
# UserObject returns the given capillary pressure for all liquid saturations.
#
# The liquid saturation is initialised to vary linearly from zero at the left
# end of the 1D mesh, to unity at the right end.
#
# #############################################################################

[UserObjects]
  [./CapillaryPressure]
    type = CapillaryPressureConstant
    cp = -1.2e5
  [../]
[]

[Functions]
  [./initial_saturation]
    type = ParsedFunction
    value = x
  [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
[]

[AuxVariables]
  [./CapillaryPressureAux]
  [../]
[]

[AuxKernels]
  [./CapillaryPressureAux]
    capillary_pressure_uo = CapillaryPressure
    type = CapillaryPressureAux
    variable = CapillaryPressureAux
    liquid_saturation_variable = sat_liq
    execute_on = initial
  [../]
[]

[Variables]
  [./sat_liq]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./Diffusion]
    type = Diffusion
    variable = sat_liq
  [../]
[]

[VectorPostprocessors]
  [./vpp]
    type = NodalValueSampler
    variable = CapillaryPressureAux
    sort_by = x
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
[]

[Outputs]
  file_base = CapillaryPressureConstant
  [./csv]
    type = CSV
  [../]
[]

[ICs]
  [./LiquidSaturationIC]
    function = initial_saturation
    variable = sat_liq
    type = FunctionIC
  [../]
[]

