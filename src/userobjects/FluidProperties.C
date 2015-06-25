/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidProperties.h"

template<>
InputParameters validParams<FluidProperties>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription("Fluid properties base class.  Override properties in your class");
  return params;
}

FluidProperties::FluidProperties(const std::string & name, InputParameters parameters) :
  GeneralUserObject(name, parameters)
{}

void
FluidProperties::initialize()
{}

void
FluidProperties::execute()
{}

void
FluidProperties::finalize()
{}

Real
FluidProperties::density(Real pressure, Real temperature) const
{
  mooseError("FluidProperties::density(Real pressure, Real temperature) unimplemented in derived fluid properties class");
  return 0.;
}

Real
FluidProperties::density(Real pressure, Real temperature, Real xmass) const
{
  mooseError("FluidProperties::density(Real pressure, Real temperature, Real xmass) unimplemented in derived fluid properties class");
  return 0.;
}

Real
FluidProperties::viscosity(Real pressure, Real temperature) const
{
  mooseError("FluidProperties::viscosity(Real pressure, Real temperature) unimplemented in derived fluid properties class");
  return 0.;
}

Real
FluidProperties::viscosity(Real pressure, Real temperature, Real xmass) const
{
  mooseError("FluidProperties::viscosity(Real pressure, Real temperature, Real xmass) unimplemented in derived fluid properties class");
  return 0.;
}

Real
FluidProperties::dDensity_dP(Real pressure, Real temperature) const
{
  mooseError("FluidProperties::dDensity_dP(Real pressure, Real temperature) unimplemented in derived fluid properties class");
  return 0.;
}

Real
FluidProperties::dDensity_dT(Real pressure, Real temperature) const
{
  mooseError("FluidProperties::dDensity_dT(Real pressure, Real temperature) unimplemented in derived fluid properties class");
  return 0.;
}
