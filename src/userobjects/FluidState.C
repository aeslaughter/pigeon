/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidState.h"

template<>
InputParameters validParams<FluidState>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription("Fluid state base class. Override properties in your class");
  params.addRequiredCoupledVar("temperature_variable", "The fluid temperature variable. For isothermal simulations, enter the fluid temperature (C)");
  params.addRequiredCoupledVar("pressure_variable", "The fluid pressure variable (Pa)");
  params.addCoupledVar("saturation_variable", 1.0, "The primary saturation variable");
  params.addCoupledVar("mass_fraction_variable", 1.0, "The primary mass fraction variable");
  params.addParam<unsigned int>("component_index", 0, "The index of the primary mass fraction component");
  params.addParam<unsigned int>("num_components", 1, "The number of components in this FluidState. Set to 1 if 'mass_fraction_variable' is 0, or 2 otherwise");

  return params;
}

FluidState::FluidState(const InputParameters & parameters) :
  GeneralUserObject(parameters),
  Coupleable(parameters, false),
  _mesh(_subproblem.mesh()),
  _qp(0),
  _pressure(coupledNodalValue("pressure_variable")),
  _temperature(coupledNodalValue("temperature_variable")),
  _saturation(coupledNodalValue("saturation_variable")),
  _mass_fraction(coupledNodalValue("mass_fraction_variable")),
  _num_components(getParam<unsigned int>("num_components")),
  _not_isothermal(isCoupled("temperature_variable")),
  _pvar(coupled("pressure_variable")),
  _tvar(coupled("temperature_variable")),
  _svar(coupled("saturation_variable")),
  _xvar(coupled("mass_fraction_variable")),
  _component_index(getParam<unsigned int>("component_index"))

{
  /// Determine the number of primary variables required to fully represent
  /// the fluid system.
  /// Pressure and temperature variables are always required
  _num_vars = 2;
  /// If saturation is coupled, increment _num_vars
  if (isCoupled("saturation_variable"))
    _num_vars ++;
  /// If mass fraction is coupled, increment _num_vars
  if (isCoupled("mass_fraction_variable"))
    _num_vars ++;

  /// Vector of FLuidState variable numbers. Pressure is always a primary variable
  _varnums.push_back(_pvar);
  /// If temperature is coupled, add tvar
  if (_not_isothermal)
    _varnums.push_back(_tvar);
  /// If saturation is coupled, add svar
  if (isCoupled("saturation_variable"))
    _varnums.push_back(_svar);
  /// If mass fraction is coupled, add xvar
  if (isCoupled("mass_fraction_variable"))
    _varnums.push_back(_xvar);

  // Get the names of the variables corresponding to the FluidState variables
  _pname = getVar("pressure_variable", 0)->name();
  if (isCoupled("temperature_variable"))
    _tname = getVar("temperature_variable", 0)->name();
  if (isCoupled("saturation_variable"))
    _sname = getVar("saturation_variable", 0)->name();
  if (isCoupled("mass_fraction_variable"))
    _xname = getVar("mass_fraction_variable", 0)->name();
}

unsigned int
FluidState::numComponents() const
{
  return _num_components;
}

bool
FluidState::isIsothermal() const
{
   return 1 - _not_isothermal;  // Returns true if isothermal
}

Real
FluidState::isothermalTemperature() const
{
  // For isothermal simulations
  return _temperature[_qp];
}

unsigned int
FluidState::primaryComponentIndex() const
{
  return _component_index;
}

bool
FluidState::isFluidStateVariable(unsigned int moose_var) const
{
  bool isvariable = true;
  if (std::find(_varnums.begin(), _varnums.end(), moose_var) == _varnums.end())
    isvariable = false;

  return isvariable;
}

unsigned int
FluidState::temperatureVar() const
{
  return _tvar;
}

std::string
FluidState::variableNames(unsigned int moose_var) const
{
  std::string varname;
  if (moose_var == _pvar)
    varname = _pname;
  else if (moose_var == _tvar)
    varname = _tname;
  else if (moose_var == _svar)
    varname = _sname;
  else if (moose_var == _xvar)
      varname = _xname;
  else
    mooseError("Variable " << moose_var << " is not a FluidState variable");

  return varname;
}

std::string
FluidState::variableTypes(unsigned int moose_var) const
{
  std::string vartype;
  if (moose_var == _pvar)
    vartype = "pressure";
  else if (moose_var == _tvar)
    vartype = "temperature";
  else if (moose_var == _xvar)
    vartype = "mass_fraction";
  else
    mooseError("Variable " << moose_var << " is not a FluidState variable");

  return vartype;
}


void
FluidState::initialize()
{
  _nodal_properties.clear();
  _primary_vars.resize(_num_vars);
}

void
FluidState::execute()
{}

void
FluidState::finalize()
{}

Real
FluidState::getNodalProperty(std::string property, unsigned int nodeid, unsigned int phase_index, unsigned int component_index) const
{
  if (phase_index >= numPhases())
    mooseError("Phase index " << phase_index << " out of range in FluidState::getNodalProperty");

  FluidStateProperties fsp;
  Real value=0;

  std::map<int, FluidStateProperties>::const_iterator node_it = _nodal_properties.find(nodeid);

  if (node_it != _nodal_properties.end())
    fsp = node_it->second;
  else
    return value; // Return zero if the node is out of range initially
  //  mooseError("Node id "<< nodeid << " out of range in FluidStateTwoPhase::getNodalProperty");

  // Now access the property and phase index
  if (property == "pressure")
    value = fsp.pressure[phase_index];
  else if (property == "saturation")
    value = fsp.saturation[phase_index];
  else if (property == "density")
    value = fsp.density[phase_index];
  else if (property == "viscosity")
    value = fsp.viscosity[phase_index];
  else if (property == "relperm")
    value = fsp.relperm[phase_index];
  else if (property == "mass_fraction")
    value = fsp.mass_fraction[component_index][phase_index];
  else if (property == "mobility")
    value = fsp.mobility[phase_index];
  else if (property == "ddensity_dp")
    value = fsp.ddensity_dp[phase_index];
  else if (property == "ddensity_ds")
    value = fsp.ddensity_ds[phase_index];
  else if (property == "ddensity_dx")
    value = fsp.ddensity_dx[component_index][phase_index];
  else if (property == "drelperm")
    value = fsp.drelperm[phase_index];
  else if (property == "dmobility_dp")
    value = fsp.dmobility_dp[phase_index];
  else if (property == "dmobility_ds")
    value = fsp.dmobility_ds[phase_index];
  else if (property == "dmobility_dx")
    value = fsp.dmobility_dx[component_index][phase_index];
  else
    mooseError("Property " << property << " in FluidState::getNodalProperty is not one of the members of the FluidStateProperties class. Check spelling of property.");

  return value;
}
