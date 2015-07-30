/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateSinglePhase.h"

template<>
InputParameters validParams<FluidStateSinglePhase>()
{
  InputParameters params = validParams<FluidState>();
  params.addClassDescription("Thermophysical fluid state of single phase fluid");
  params.addRequiredParam<UserObjectName>("fluid_property_uo", "Name of the User Object defining the fluid properties");
  params.addRequiredCoupledVar("temperature_variable", "The fluid temperature variable. For isothermal simulations, enter the fluid temperature (C)");
  params.addRequiredCoupledVar("pressure_variable", "The fluid pressure variable (Pa)");
  params.addRequiredCoupledVar("mass_fraction_variable", "The mass fraction of the dissolved component. Set to 0 for no dissolved component");
  params.addParam<unsigned int>("component_index", 0, "The index of the primary mass fraction component");
  params.addParam<unsigned int>("num_components", 1, "The number of components in this FluidState. Set to 1 if 'mass_fraction_variable' is 0, or 2 otherwise");
  params.addParam<Real>("component_density_increase", 0., "The increase in density with dissolved component 1");
  return params;
}

FluidStateSinglePhase::FluidStateSinglePhase(const InputParameters & parameters) :
  FluidState(parameters),

  _fluid_property(getUserObject<FluidProperties>("fluid_property_uo")),
  _pressure(coupledNodalValue("pressure_variable")),
  _temperature(coupledNodalValue("temperature_variable")),
  _mass_fraction(coupledNodalValue("mass_fraction_variable")),
  _num_components(getParam<unsigned int>("num_components")),
  _not_isothermal(isCoupled("temperature_variable")),
  _pvar(coupled("pressure_variable")),
  _tvar(coupled("temperature_variable")),
  _xvar(coupled("mass_fraction_variable")),
  _component_index(getParam<unsigned int>("component_index")),
  _density_increase(getParam<Real>("component_density_increase"))

{
  _num_phases = 1;
  _num_vars = 3;

  _varnums.push_back(_pvar);
  if (_not_isothermal)
    _varnums.push_back(_tvar);
  if (isCoupled("mass_fraction_variable"))
    _varnums.push_back(_xvar);

  // Get the names of the variables
  if (isCoupled("pressure_variable"))
    _pname = getVar("pressure_variable", 0)->name();
  if (isCoupled("temperature_variable"))
    _tname = getVar("temperature_variable", 0)->name();
  if (isCoupled("mass_fraction_variable"))
    _xname = getVar("mass_fraction_variable", 0)->name();

}

unsigned int
FluidStateSinglePhase::numPhases() const
{
  return _num_phases;
}

unsigned int
FluidStateSinglePhase::numComponents() const
{
  return _num_components;
}

bool
FluidStateSinglePhase::isIsothermal() const
{
   return 1 - _not_isothermal;  // Returns true if isothermal
}

Real
FluidStateSinglePhase::isothermalTemperature() const
{
  // For isothermal simulations
  return _temperature[_qp];
}

unsigned int
FluidStateSinglePhase::temperatureVar() const
{
  return _tvar;
}

bool
FluidStateSinglePhase::isFluidStateVariable(unsigned int moose_var) const
{
  bool isvariable = true;
  if (std::find(_varnums.begin(), _varnums.end(), moose_var) == _varnums.end())
    isvariable = false;

  return isvariable;
}

std::string
FluidStateSinglePhase::variableNames(unsigned int moose_var) const
{
  std::string varname;
  if (moose_var == _pvar)
    varname = _pname;
  if (moose_var == _tvar)
    varname = _tname;
  if (moose_var == _xvar)
      varname = _xname;

  return varname;
}

std::string
FluidStateSinglePhase::variableTypes(unsigned int moose_var) const
{
  std::string vartype;
  if (moose_var == _pvar)
    vartype = "pressure";
  if (moose_var == _tvar)
    vartype = "temperature";
  if (moose_var == _xvar)
    vartype = "mass_fraction";

  return vartype;
}

unsigned int
FluidStateSinglePhase::variablePhase(unsigned int moose_var) const
{
 // Only one phase in this FluidState
  return 0;
}

unsigned int
FluidStateSinglePhase::primaryComponentIndex() const
{
  return _component_index;
}

void
FluidStateSinglePhase::initialize()
{
  _nodal_properties.clear();
  _primary_vars.resize(_num_vars);
}

void
FluidStateSinglePhase::execute()
{
  MooseVariable * pvar, * tvar, * xvar;
  FluidStateProperties nodalfsp;

  if (isCoupled("pressure_variable"))
    pvar = getVar("pressure_variable", 0);
  if(isCoupled("temperature_variable"))
    tvar = getVar("temperature_variable", 0);
  if(isCoupled("mass_fraction_variable"))
    xvar = getVar("mass_fraction_variable", 0);

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

        if (isCoupled("temperature_variable"))
          _primary_vars[1] = tvar->getNodalValue(*current_node);
        else
          _primary_vars[1] = _temperature[_qp];

        if (isCoupled("mass_fraction_variable"))
          _primary_vars[2] = xvar->getNodalValue(*current_node);
        else
          _primary_vars[2] = _mass_fraction[_qp];

        // Now calculate all thermophysical properties at the current node
        thermophysicalProperties(_primary_vars, nodalfsp);

        // Now insert these properties into the _nodal_properties map
        _nodal_properties.insert( std::pair<int, FluidStateProperties>(nodeid, nodalfsp));
      }
    }
  }
}

void
FluidStateSinglePhase::thermophysicalProperties(std::vector<Real> primary_vars, FluidStateProperties & fsp)
{
  // Primary variables at the node
  Real node_pressure = primary_vars[0];
  Real node_temperature = primary_vars[1];
  Real node_xmass = primary_vars[2];

  // Assign the fluid properties
  // Saturation
  Real node_saturation = 1.0;
  fsp.saturation = saturation(node_saturation);

  // Pressure (takes liquid saturation for capillary pressure calculation)
  fsp.pressure = pressure(node_pressure, fsp.saturation[0]);

  // Density FIXME: Need to generalise density of saturated mixture
  std::vector<Real> densities(_num_phases);
  Real density0 = _fluid_property.density(fsp.pressure[0], node_temperature);
  /// Now add increase due to dissolved component 1
  densities[0] = density0 * (density0 + _density_increase) / (density0 + _density_increase - node_xmass * _density_increase);

  fsp.density = densities;

  // Viscosity
  std::vector<Real> viscosities(_num_phases);

  // Water viscosity uses water density in calculation.
  // FIXME: make this general for temp, pressure, density
  viscosities[0] = _fluid_property.viscosity(node_temperature, fsp.density[0]);

  fsp.viscosity = viscosities;

  // Relative permeability (Equal to 1)
  fsp.relperm = relativePermeability(fsp.saturation[0]);

  // Mass fraction of each component
  std::vector<std::vector<Real> > xmass;
  xmass.resize(_num_components);

  xmass[_component_index].push_back(node_xmass); // primary mass fraction
  for (unsigned int i = 0; i < _num_components; ++i)
    if (i != _component_index)
      xmass[i].push_back(1.0 - node_xmass); // Other mass fraction

  fsp.mass_fraction = xmass;

  // Mobility
  std::vector<Real> mobilities(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
    mobilities[n] = fsp.relperm[n] * fsp.density[n]  / fsp.viscosity[n];

  fsp.mobility = mobilities;

  // For derivatives wrt saturation, the sign of the derivative depends on whether the primary
  // saturation variable is liquid or not. This can be accounted for by multiplying all derivatives
  // wrt S by dS/dSl
  Real sgn = 1.0;

  // Derivative of relative permeability wrt liquid_saturation
  std::vector<Real> drelperm(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
    drelperm[n] = sgn * dRelativePermeability(fsp.saturation[0])[n];

  fsp.drelperm = drelperm;

  // Derivative of density wrt pressure
  std::vector<Real> ddensities_dp(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
    ddensities_dp[n] = dDensity_dP(fsp.pressure[n], node_temperature, n);

  fsp.ddensity_dp = ddensities_dp;

  // Derivative of density wrt saturation
  std::vector<Real> ddensities_ds(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
    ddensities_ds[n] = sgn * dCapillaryPressure(fsp.saturation[0])[n] * fsp.ddensity_dp[n];

  fsp.ddensity_ds = ddensities_ds;

  // Derivative of density wrt mass fraction
  std::vector<std::vector<Real> > ddensities_dx;
  Real sgnx;
  ddensities_dx.resize(numComponents());
//FIXME: need to fix this up properly
  Real ddx = _density_increase * fsp.density[0] / (density0 + _density_increase - node_xmass * _density_increase);
  for (unsigned int i = 0; i < _num_components; ++i)
  {
    sgnx = dMassFraction_dX(i);
    ddensities_dx[i].push_back(sgnx * ddx);
  }

  fsp.ddensity_dx = ddensities_dx;

  // Derivative of mobility wrt pressure
  // Note: dViscosity_dP not implemnted yet
  std::vector<Real> dmobilities_dp(_num_phases);
  Real dmdp;

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    dmdp = (fsp.relperm[n] * fsp.ddensity_dp[n] / fsp.viscosity[n]) * (1.0 - (fsp.density[n] / fsp.viscosity[n]) *
      dViscosity_dDensity(fsp.density[n], node_temperature, n));
    dmobilities_dp[n] = dmdp;
  }

  fsp.dmobility_dp = dmobilities_dp;

  // Derivative of mobility wrt saturation
  // Note: dViscosity_dS not implemnted yet
  // Note: drelperm and ddensity_ds are already the correct sign, so don't multiply by sgn
  std::vector<Real> dmobilities_ds(_num_phases);
  Real dmds;
  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    dmds = fsp.drelperm[n] * fsp.density[n] / fsp.viscosity[n] + (fsp.relperm[n] * fsp.ddensity_ds[n] / fsp.viscosity[n]) * (1.0 - (fsp.density[n] / fsp.viscosity[n]) *
      dViscosity_dDensity(fsp.density[n], node_temperature, n));
    dmobilities_ds[n] = dmds;
  }

  fsp.dmobility_ds = dmobilities_ds;

  // Derivative of mobility wrt saturation
  // Note: ddensity_dx is already the correct sign, so don't multiply by sgn
  std::vector<std::vector<Real> > dmobilities_dx(_num_components);
  Real dmdx;
  for (unsigned int i = 0; i < _num_components; ++i)
    for (unsigned int n = 0; n < _num_phases; ++n)
    {
      dmdx = (fsp.relperm[n] * fsp.ddensity_dx[i][n] / fsp.viscosity[n]) * (1.0 - (fsp.density[n] / fsp.viscosity[n]) *
        dViscosity_dDensity(fsp.density[n], node_temperature, n));
      dmobilities_dx[i].push_back(dmdx);
    }
  fsp.dmobility_dx = dmobilities_dx;
}

Real
FluidStateSinglePhase::density(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_density = _fluid_property.density(pressure, temperature);

  return fluid_density;
}

Real
FluidStateSinglePhase::viscosity(Real pressure, Real temperature, unsigned int phase_index) const
{
  // TODO: Fix this so that density isn't calculated twice.
  Real fluid_density = _fluid_property.density(pressure, temperature);
  Real fluid_viscosity = _fluid_property.viscosity(temperature, fluid_density);

  return fluid_viscosity;
}

std::vector<std::vector<Real> >
FluidStateSinglePhase::massFractions(Real pressure, Real temperature) const
{
  std::vector<std::vector<Real> > xmass;
  unsigned int numcomp = numComponents();
  // unsigned int numphase = numPhases();

  xmass.resize(numcomp);

  xmass[0].push_back(1.0); // Component in phase
  xmass[0].push_back(0.0); // No other phase

  return xmass;
}

std::vector<Real>
FluidStateSinglePhase::relativePermeability(Real liquid_saturation) const
{
  std::vector<Real> relperm;

  relperm.push_back(1.0);

  return relperm;
}

std::vector<Real>
FluidStateSinglePhase::dRelativePermeability(Real liquid_saturation) const
{
  std::vector<Real> drelperm;

  drelperm.push_back(0.0);

  return drelperm;
}

std::vector<Real>
FluidStateSinglePhase::pressure(Real pressure, Real liquid_saturation) const
{
  std::vector<Real> pressures;

  pressures.push_back(pressure);

  return pressures;
}

std::vector<Real>
FluidStateSinglePhase::dCapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> dpc;

  dpc.push_back(0.0);

  return dpc;
}

std::vector<Real>
FluidStateSinglePhase::saturation(Real saturation) const
{
  std::vector<Real> saturations;

  saturations.push_back(saturation);

  return saturations;
}

Real
FluidStateSinglePhase::dSaturation_dSl(unsigned int phase_index) const
{
  return 1.0;
}

Real
FluidStateSinglePhase::dSaturation_dS(unsigned int var) const
{
  return 1.0;
}

Real
FluidStateSinglePhase::dMassFraction_dX(unsigned int component_index) const
{
  return (component_index == _component_index ? 1.0 : -1.0);
}

Real
FluidStateSinglePhase::dDensity_dP(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real dfluid_density = _fluid_property.dDensity_dP(pressure, temperature);

  return dfluid_density;
}

Real
FluidStateSinglePhase::dDensity_dX(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real dfluid_density = 0.0;

  return dfluid_density;
}

Real
FluidStateSinglePhase::dViscosity_dDensity(Real density, Real temperature, unsigned int phase_index) const
{
  Real dviscosity_ddensity = _fluid_property.dViscosity_dDensity(temperature, density);

  return dviscosity_ddensity;
}
