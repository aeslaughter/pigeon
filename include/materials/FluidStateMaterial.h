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

//Forward Declarations
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
  MaterialProperty<RealVectorValue> & _gravity;
  MaterialProperty<std::vector<RealGradient> > & _phase_flux_no_mobility;
  MaterialProperty<std::vector<Real> > & _phase_mass;

  VariableValue & _primary_saturation;
  VariableGradient & _grad_primary_saturation;
  VariableValue & _primary_pressure;
  VariableGradient & _grad_primary_pressure;
  VariableValue & _temperature;
  unsigned int _num_phases;
  unsigned int _phase_index;

 /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;

};

#endif //FLUIDSTATEMATERAIL_H
