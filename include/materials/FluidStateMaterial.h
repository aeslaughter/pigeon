/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATEMATERIAL_H
#define FLUIDSTATEMATERIAL_H

#include "Material.h"
#include "PorousMaterial.h"
#include "FluidState.h"

class FluidStateMaterial;

template<>
InputParameters validParams<FluidStateMaterial>();

/**
 * General fluid state material class for multiphase flow through porous media.
 */
class FluidStateMaterial : public Material
{
public:
  FluidStateMaterial(const std::string & name,
                  InputParameters parameters);

protected:
  virtual void computeQpProperties();

private:
  const MaterialProperty<RealVectorValue> & _gravity;
  MaterialProperty<std::vector<RealGradient> > & _phase_flux_no_mobility;
  MaterialProperty<std::vector<Real> > & _phase_mass;
  MaterialProperty<std::vector<Real> > & _dpressure_flux_dp;
  MaterialProperty<std::vector<Real> > & _dpressure_flux_ds;
  MaterialProperty<std::vector<RealVectorValue> > & _dgravity_flux_dp;
  MaterialProperty<std::vector<RealVectorValue> > & _dgravity_flux_ds;


  VariableValue & _primary_saturation;
  VariableGradient & _grad_primary_saturation;
  VariableValue & _primary_pressure;
  VariableGradient & _grad_primary_pressure;
  VariableValue & _temperature;
  std::vector<VariableValue *> _density;
  unsigned int _num_phases;
  unsigned int _phase_index;
  unsigned int _pvar;
  unsigned int _svar;

 /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;

  unsigned int _p_phase;
  unsigned int _s_phase;

};

#endif //FLUIDSTATEMATERAIL_H