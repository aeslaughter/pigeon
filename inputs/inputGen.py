#/usr/local/bin/python3
#helps create all the data that is used in calc + just passes through mesh
varNames = ['power','volPower','flux1g','flux1g_err','flux2g','flux2g_err','flux3g','flux3g_err','flux4g','flux4g_err','keff','stdkeff']

def auxVariables(varNames):
    for var in varNames:
        print('[./auxVar_{0}]'.format(var))
        print('  block = \'1 2 3\'')
        print('  family = MONOMIAL')
        print('  order = CONSTANT')
        print('[../]')
        print('')
        
def auxKernels(varNames):
    for var in varNames:
        print('[./auxKernel_{0}]'.format(var))
        print('  type = SolutionAux')
        print('  variable = auxVar_{0}'.format(var))
        print('  block = \'1 2 3\'')
        print('  solution = {0}UO'.format(var))
        print('  execute_on = timestep_begin')
        print('[../]')
        print('')
        
def userObjects(varNames):
    meshFile = 'output_cyl.e'
    for var in varNames:
        print('[./{0}UO]'.format(var))
        print('  type = SolutionUserObject')
        print('  mesh = {0}'.format(meshFile))
        print('  system_variables = \'{0}\''.format(var))
        print('  timestep = 1')
        print('[../]')
        print('')
        
auxVariables(varNames)        
auxKernels(varNames)
userObjects(varNames)


  # [./ProjectTempOnAux]
  #   type = ProjectVarAux
  #   block = '1 2 3'
  #   execute_on = timestep
  #   variable = elementalTemp
  #   coupled = T
  # [../]
  
  # [./auxKernel_power]
  #   type = SolutionAux
  #   variable = auxVar_power
  #   block = '1 2 3'
  #   solution = powerUO
  #   execute_on = timestep_begin
  # [../]