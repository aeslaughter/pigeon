/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateTwoPhase.h"

template<>
InputParameters validParams<FluidStateTwoPhase>()
{
  InputParameters params = validParams<FluidState>();
  params.addClassDescription("Thermophysical fluid state of two-phase sliquid and gas.");
  params.addRequiredParam<UserObjectName>("liquid_property_uo", "Name of the User Object defining the liquid properties");
  params.addRequiredParam<UserObjectName>("gas_property_uo", "Name of the User Object defining the gas properties");
  params.addRequiredParam<UserObjectName>("relative_permeability_uo", "Name of the User Object defining the relative permeabilities");
  params.addRequiredParam<UserObjectName>("capillary_pressure_uo", "Name of the User Object defining the capillary pressure");
  params.addRequiredCoupledVar("pressure_variable",  "The primary pressure variable");
  params.addRequiredCoupledVar("temperature_variable", "The primary temperature variable.");
  params.addRequiredCoupledVar("saturation_variable", "The primary saturation variable");
  params.addParam<unsigned int>("pressure_variable_phase", 0, "The phase corresponding to the pressure variable");
  params.addParam<unsigned int>("saturation_variable_phase", 0, "The phase corresponding to the saturation variable");
  return params;
}

FluidStateTwoPhase::FluidStateTwoPhase(const InputParameters & parameters) :
  FluidState(parameters),

  _liquid_property(getUserObject<FluidProperties>("liquid_property_uo")),
  _gas_property(getUserObject<FluidProperties>("gas_property_uo")),
  _relative_permeability(getUserObject<RelativePermeability>("relative_permeability_uo")),
  _capillary_pressure(getUserObject<CapillaryPressure>("capillary_pressure_uo")),
  _pressure(coupledNodalValue("pressure_variable")),
  _temperature(coupledNodalValue("temperature_variable")),
  _saturation(coupledNodalValue("saturation_variable")),
  _not_isothermal(isCoupled("temperature_variable")),
  _p_phase(getParam<unsigned int>("pressure_variable_phase")),
  _s_phase(getParam<unsigned int>("saturation_variable_phase")),
  _pvar(coupled("pressure_variable")),
  _svar(coupled("saturation_variable")),
  _tvar(coupled("temperature_variable"))

{
  _num_components = 2;
  _num_phases = 2;
  _num_vars = _coupled_moose_vars.size() + (1 - _not_isothermal);

  _varnums.push_back(_pvar);
  _varnums.push_back(_svar);
  if (_not_isothermal)
   _varnums.push_back(_tvar);

  // The pressure variable must always be coupled
  _pname = getVar("pressure_variable", 0)->name();
  // The saturation variable must always be coupled
  _sname = getVar("saturation_variable", 0)->name();
  if (_not_isothermal) // Check if temperature is coupled
    _tname = getVar("temperature_variable", 0)->name();
}

unsigned int
FluidStateTwoPhase::numPhases() const
{
  return _num_phases;
}

unsigned int
FluidStateTwoPhase::numComponents() const
{
  return _num_components;
}

bool
FluidStateTwoPhase::isIsothermal() const
{
   return 1 - _not_isothermal;  // Returns true if isothermal
}

Real
FluidStateTwoPhase::isothermalTemperature() const
{
  // For isothermal simulations
  return _temperature[_qp];
}

unsigned int
FluidStateTwoPhase::temperatureVar() const
{
  return _tvar;
}

bool
FluidStateTwoPhase::isFluidStateVariable(unsigned int moose_var) const
{
  bool isvariable = true;
  if (std::find(_varnums.begin(), _varnums.end(), moose_var) == _varnums.end())
    isvariable = false;

  return isvariable;
}
std::string
FluidStateTwoPhase::variableNames(unsigned int moose_var) const
{
  std::string varname;

  if (moose_var == _pvar)
    varname = _pname;
  if (moose_var == _svar)
    varname = _sname;
  if (moose_var == _tvar)
    varname = _tname;

  return varname;
}

std::string
FluidStateTwoPhase::variableTypes(unsigned int moose_var) const
{
  std::string vartype;

  if (moose_var == _pvar)
    vartype = "pressure";
  if (moose_var == _svar)
    vartype = "saturation";
  if (moose_var == _tvar)
    vartype = "temperature";

  return vartype;
}

unsigned int
FluidStateTwoPhase::variablePhase(unsigned int moose_var) const
{
  unsigned int varphase;

  if (moose_var == _pvar)
    varphase = _p_phase;
  if (moose_var == _svar)
    varphase = _s_phase;

  return varphase;
}

void
FluidStateTwoPhase::initialize()
{
  _nodal_properties.clear();
  _primary_vars.resize(_num_vars);
}

void
FluidStateTwoPhase::execute()
{
  MooseVariable * pvar, * svar, * tvar;
  FluidStateProperties nodalfsp;

  if (isCoupled("pressure_variable"))
    pvar = getVar("pressure_variable", 0);
  if (isCoupled("saturation_variable"))
    svar = getVar("saturation_variable", 0);
  if(isCoupled("temperature_variable"))
    tvar = getVar("temperature_variable", 0);

  // Loop over all elements on current processor
  const MeshBase::element_iterator end = _mesh.getMesh().active_local_elements_end();
  for (MeshBase::element_iterator el = _mesh.getMesh().active_local_elements_begin(); el != end; ++el)
  {
    const Elem * current_elem = *el;

    // Loop over all nodes on each element
    for (unsigned int i = 0; i < current_elem->n_vertices(); ++i)
    {
      const Node * current_node = current_elem->get_node(i);
      unsigned int nodeid = current_node->id();

      // Check if the properties at this node have already been calcualted, and if so,
      // skip to the next node
      if (_nodal_properties.find(nodeid) == _nodal_properties.end())
      {
        if (isCoupled("pressure_variable"))
          _primary_vars[0] = pvar->getNodalValue(*current_node);
        else
          _primary_vars[0] = _pressure[_qp];

        if (isCoupled("saturation_variable"))
          _primary_vars[1] = svar->getNodalValue(*current_node);
        else
          _primary_vars[1] = _saturation[_qp];

        if (isCoupled("temperature_variable"))
          _primary_vars[2] = tvar->getNodalValue(*current_node);
        else
          _primary_vars[2] = _temperature[_qp];

        // Now calculate all thermophysical properties at the current node
        thermophysicalProperties(_primary_vars, nodalfsp);

        // Now insert these properties into the _nodal_properties map
        _nodal_properties.insert( std::pair<int, FluidStateProperties>(nodeid, nodalfsp));
      }
    }
  }
}

void
FluidStateTwoPhase::thermophysicalProperties(std::vector<Real> primary_vars, FluidStateProperties & fsp)
{
  // Primary variables at the node
  Real node_pressure = primary_vars[0];
  Real node_saturation = primary_vars[1];
  Real node_temperature = primary_vars[2];

  // Assign the fluid properties
  // Saturation
  fsp.saturation = saturation(node_saturation);

  // Pressure (takes liquid saturation for capillary pressure calculation)
  fsp.pressure = pressure(node_pressure, fsp.saturation[0]);

  // Density of each phase
  std::vector<Real> densities(_num_phases);

  densities[0] = _liquid_property.density(fsp.pressure[0], node_temperature);
  densities[1] = _gas_property.density(fsp.pressure[1], node_temperature);

  fsp.density = densities;

  // Viscosity of each phase
  std::vector<Real> viscosities(_num_phases);

  // Water viscosity uses water density in calculation.
  viscosities[0] = _liquid_property.viscosity(node_temperature, fsp.density[0]);
  viscosities[1] = _gas_property.viscosity(fsp.pressure[1], node_temperature);

  fsp.viscosity = viscosities;

  // Relative permeability of each phase
  fsp.relperm = relativePermeability(fsp.saturation[0]);

  // Mass fraction of each component in each phase
  std::vector<std::vector<Real> > xmass;
  xmass.resize(numComponents());

  xmass[0].push_back(1.0); // Only liquid component in liquid
  xmass[0].push_back(0.0); // No liquid component in gas
  xmass[1].push_back(0.0); // No gas component in liquid
  xmass[1].push_back(1.0); // Only gas component in gas

  fsp.mass_fraction = xmass;

  // Mobility of each phase
  std::vector<Real> mobilities(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    mobilities[i] = fsp.relperm[i] * fsp.density[i]  / fsp.viscosity[i];

  fsp.mobility = mobilities;

  // For derivatives wrt saturation, the sign of the derivative depends on whether the primary
  // saturation variable is liquid or not. This can be accounted for by multiplying all derivatives
  // wrt S by dS/dSl
  Real sgn = dSaturation_dSl(_svar);

  // Derivative of relative permeability wrt liquid_saturation
  std::vector<Real> drelperm(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    drelperm[i] = sgn * dRelativePermeability(fsp.saturation[0])[i];

  fsp.drelperm = drelperm;

  // Derivative of density wrt pressure
  std::vector<Real> ddensities_dp(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    ddensities_dp[i] = dDensity_dP(fsp.pressure[i], node_temperature, i);

  fsp.ddensity_dp = ddensities_dp;

  // Derivative of density wrt saturation
  std::vector<Real> ddensities_ds(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    ddensities_ds[i] = sgn * dCapillaryPressure(fsp.saturation[0])[i] * fsp.ddensity_dp[i];

  fsp.ddensity_ds = ddensities_ds;

  // Derivative of mobility wrt pressure
  // Note: dViscosity_dP not implemnted yet
  std::vector<Real> dmobilities_dp(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    dmobilities_dp[i] = fsp.relperm[i] * fsp.ddensity_dp[i]  / fsp.viscosity[i];

  fsp.dmobility_dp = dmobilities_dp;

  // Derivative of mobility wrt saturation
  // Note: dViscosity_dS not implemnted yet
  // Note: drelperm and ddensity_ds are already the correct sign, so don't multiply by sgn
  std::vector<Real> dmobilities_ds(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    dmobilities_ds[i] = (fsp.drelperm[i] * fsp.density[i] + fsp.relperm[i] *
      fsp.ddensity_ds[i]) / fsp.viscosity[i];

  fsp.dmobility_ds = dmobilities_ds;
}

Real
FluidStateTwoPhase::getNodalProperty(std::string property, unsigned int nodeid, unsigned int phase_index, unsigned int component_index) const
{
  if (phase_index >= _num_phases)
    mooseError("Phase index " << phase_index << " out of range in FluidStateTwoPhase::getNodalProperty");

  FluidStateProperties fsp;
  Real value=0;
  std::map<int, FluidStateProperties>::const_iterator node_it = _nodal_properties.find(nodeid);

  if (node_it != _nodal_properties.end())
    fsp = node_it->second;
  else
    return value;  // FIXME: Sometimes the AuxKernel was trying to access data before execute above was run
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
    value = fsp.mass_fraction[phase_index][component_index];
  else if (property == "mobility")
    value = fsp.mobility[phase_index];
  else if (property == "ddensity_dp")
    value = fsp.ddensity_dp[phase_index];
  else if (property == "ddensity_ds")
    value = fsp.ddensity_ds[phase_index];
  else if (property == "drelperm")
    value = fsp.drelperm[phase_index];
  else if (property == "dmobility_dp")
    value = fsp.dmobility_dp[phase_index];
  else if (property == "dmobility_ds")
    value = fsp.dmobility_ds[phase_index];
  else
    mooseError("Property " << property << " in FluidStateTwoPhase::getNodalProperty is not one of the members of the FluidStateProperties class. Check spelling of property.");

  return value;
}

unsigned int
FluidStateTwoPhase::getPrimarySaturationVar() const
{
  return _svar;
}

Real
FluidStateTwoPhase::density(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_density;

  if (phase_index == 0) // Liquid phase
  fluid_density = _liquid_property.density(pressure, temperature);

  else if (phase_index == 1) // Gas phase
    fluid_density = _gas_property.density(pressure, temperature);

  else
    mooseError("phase_index is out of range in FluidStateMultiphase::density");

  return fluid_density;
}

Real
FluidStateTwoPhase::viscosity(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_viscosity;

  // TODO: Fix this so that density isn't calculated twice.
  if (phase_index == 0)
  {
    Real liquid_density = _liquid_property.density(pressure, temperature);
    fluid_viscosity = _liquid_property.viscosity(temperature, liquid_density);
  }
  else if (phase_index == 1)
    fluid_viscosity = _gas_property.viscosity(pressure, temperature);

  else
    mooseError("phase_index is out of range in FluidStateTwoPhase::viscosity");

  // TODO: effect of co2 in water on viscosity

  return fluid_viscosity;
}

std::vector<std::vector<Real> >
FluidStateTwoPhase::massFractions(Real pressure, Real temperature) const
{
  std::vector<std::vector<Real> > xmass;
  unsigned int numcomp = numComponents();
  unsigned int numphase = numPhases();

  xmass.resize(numcomp);

  xmass[0].push_back(1.0); // Only liquid component in liquid
  xmass[0].push_back(0.0); // No liquid component in gas
  xmass[1].push_back(0.0); // No gas component in liquid
  xmass[1].push_back(1.0); // Only gas component in gas

  return xmass;
}

std::vector<Real>
FluidStateTwoPhase::relativePermeability(Real liquid_saturation) const
{
  std::vector<Real> relperm;

  relperm.push_back(_relative_permeability.relativePermLiquid(liquid_saturation));
  relperm.push_back(_relative_permeability.relativePermGas(liquid_saturation));

  return relperm;
}

std::vector<Real>
FluidStateTwoPhase::dRelativePermeability(Real liquid_saturation) const
{
  std::vector<Real> drelperm;

  drelperm.push_back(_relative_permeability.dRelativePermLiquid(liquid_saturation));
  drelperm.push_back(_relative_permeability.dRelativePermLiquid(liquid_saturation));

  return drelperm;
}

std::vector<Real>
FluidStateTwoPhase::pressure(Real pressure, Real liquid_saturation) const
{
  std::vector<Real> pressures;
  Real capillary_pressure = _capillary_pressure.capillaryPressure(liquid_saturation);

  // Primary pressure is liquid: Pg = Pl - Pc
  if (_p_phase == 0)
  {
    pressures.push_back(pressure);
    pressures.push_back(pressure - capillary_pressure);
  }
  // Primary pressure is gas: Pl = Pg + Pc
  if (_p_phase == 1)
  {
    pressures.push_back(pressure + capillary_pressure);
    pressures.push_back(pressure);
  }

  return pressures;
}

std::vector<Real>
FluidStateTwoPhase::dCapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> dpc;

  // Primary pressure is liquid: Pg = Pl - Pc
  if (_p_phase == 0)
  {
  dpc.push_back(0.);
  dpc.push_back(- _capillary_pressure.dCapillaryPressure(liquid_saturation));
  }

  // Primary pressure is gas: Pl = Pg + Pc
  if (_p_phase == 1)
  {
  dpc.push_back(_capillary_pressure.dCapillaryPressure(liquid_saturation));
  dpc.push_back(0.);
  }
  return dpc;
}

std::vector<Real>
FluidStateTwoPhase::saturation(Real saturation) const
{
  std::vector<Real> saturations;

  // Primary saturation is liquid
  if (_s_phase == 0)
  {
    saturations.push_back(saturation);
    saturations.push_back(1.0 - saturation);
  }
  // Else the primary saturation is gas
  if (_s_phase == 1)
  {
    saturations.push_back(1.0 - saturation);
    saturations.push_back(saturation);
  }

  return saturations;
}

Real
FluidStateTwoPhase::dSaturation_dS(unsigned int phase_index) const
{
  Real sgn = 1.0;

  // If primary saturation is not liquid
  if (phase_index != 0)
    sgn *= -1.0;

  return sgn;
}

Real
FluidStateTwoPhase::dSaturation_dSl(unsigned int var) const
{
  Real sgn = 1.0;

  // If saturation is not the primary saturation variable
  if (var != _svar || variablePhase(var) != 0)
    sgn *= -1.0;

  return sgn;
}

Real
FluidStateTwoPhase::dDensity_dP(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real dfluid_density;

  if (phase_index == 0) // liquid phase
    dfluid_density = _liquid_property.dDensity_dP(pressure, temperature);
  if (phase_index == 1) //gas phase
    dfluid_density = _gas_property.dDensity_dP(pressure, temperature);

  return dfluid_density;
}

Real
FluidStateTwoPhase::henry(Real temperature) const
{
  return 0.;
}

Real
FluidStateTwoPhase::dissolved(Real pressure, Real temperature) const
{
  return 0.;
}

/*
// Previous version that used the nodeid and filled the map inside this function
void
FluidStateTwoPhase::thermophysicalProperties(const unsigned int nodeid, std::vector<Real> primary_vars)
{
  FluidStateProperties fsp;

  // Primary variables at the node
  Real node_pressure = primary_vars[0];
  Real node_saturation = primary_vars[1];
  Real node_temperature = primary_vars[2];

  // Assign the fluid properties
  // Saturation
  fsp.saturation = saturation(node_saturation);

  // Pressure (takes liquid saturation for capillary pressure calculation)
  fsp.pressure = pressure(node_pressure, fsp.saturation[0]);

  // Density of each phase
  std::vector<Real> densities(_num_phases);

  densities[0] = _liquid_property.density(fsp.pressure[0], node_temperature);
  densities[1] = _gas_property.density(fsp.pressure[1], node_temperature);

  fsp.density = densities;

  // Viscosity of each phase
  std::vector<Real> viscosities(_num_phases);

  // Water viscosity uses water density in calculation.
  viscosities[0] = _liquid_property.viscosity(node_temperature, fsp.density[0]);
  viscosities[1] = _gas_property.viscosity(fsp.pressure[1], node_temperature);

  fsp.viscosity = viscosities;

  // Relative permeability of each phase
  fsp.relperm = relativePermeability(fsp.saturation[0]);

  // Mass fraction of each component in each phase
  std::vector<std::vector<Real> > xmass;
  xmass.resize(numComponents());

  xmass[0].push_back(1.0); // Only liquid component in liquid
  xmass[0].push_back(0.0); // No liquid component in gas
  xmass[1].push_back(0.0); // No gas component in liquid
  xmass[1].push_back(1.0); // Only gas component in gas

  fsp.mass_fraction = xmass;

  // Mobility of each phase
  std::vector<Real> mobilities(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    mobilities[i] = fsp.relperm[i] * fsp.density[i]  / fsp.viscosity[i];

  fsp.mobility = mobilities;

  // For derivatives wrt saturation, the sign of the derivative depends on whether the primary
  // saturation variable is liquid or not. This can be accounted for by multiplying all derivatives
  // wrt S by dS/dSl
  Real sgn = dSaturation_dSl(_svar);

  // Derivative of relative permeability wrt liquid_saturation
  std::vector<Real> drelperm(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    drelperm[i] = sgn * dRelativePermeability(fsp.saturation[0])[i];

  fsp.drelperm = drelperm;

  // Derivative of density wrt pressure
  std::vector<Real> ddensities_dp(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    ddensities_dp[i] = dDensity_dP(fsp.pressure[i], node_temperature, i);

  fsp.ddensity_dp = ddensities_dp;

  // Derivative of density wrt saturation
  std::vector<Real> ddensities_ds(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    ddensities_ds[i] = sgn * dCapillaryPressure(fsp.saturation[0])[i] * fsp.ddensity_dp[i];

  fsp.ddensity_ds = ddensities_ds;

  // Derivative of mobility wrt pressure
  // Note: dViscosity_dP not implemnted yet
  std::vector<Real> dmobilities_dp(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    dmobilities_dp[i] = fsp.relperm[i] * fsp.ddensity_dp[i]  / fsp.viscosity[i];

  fsp.dmobility_dp = dmobilities_dp;

  // Derivative of mobility wrt saturation
  // Note: dViscosity_dS not implemnted yet
  // Note: drelperm and ddensity_ds are already the correct sign, so don't multiply by sgn
  std::vector<Real> dmobilities_ds(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)

    dmobilities_ds[i] = (fsp.drelperm[i] * fsp.density[i] + fsp.relperm[i] *
      fsp.ddensity_ds[i]) / fsp.viscosity[i];

  fsp.dmobility_ds = dmobilities_ds;

  // Now insert these properties into the _nodal_properties map
  _nodal_properties.insert( std::pair<int, FluidStateProperties>(nodeid, fsp));
}
*/
