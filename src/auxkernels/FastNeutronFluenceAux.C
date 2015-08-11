#include "FastNeutronFluenceAux.h"

#include "Material.h"

template<>
InputParameters validParams<FastNeutronFluenceAux>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("fast_neutron_flux", "Coupled Fast Neutron Flux");
  return params;
}

FastNeutronFluenceAux::FastNeutronFluenceAux(const InputParameters & parameters)
  :AuxKernel(parameters),
   _fast_neutron_flux(coupledValue("fast_neutron_flux")),
   _fast_neutron_flux_old(coupledValueOld("fast_neutron_flux"))
{}

Real
FastNeutronFluenceAux::computeValue()
{
  return _u_old[_qp] + 0.5*_dt*(_fast_neutron_flux[_qp] + _fast_neutron_flux_old[_qp]);
}
