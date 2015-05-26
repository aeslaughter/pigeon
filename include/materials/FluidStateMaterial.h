/****************************************************************/
/* Fluid state material for multiphase flow                     */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
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
  MaterialProperty<RealVectorValue> & _gas_flux_no_mobility;
  MaterialProperty<RealVectorValue> & _liquid_flux_no_mobility;

  /**
   * The member reference of the liquid saturation variable
   */
  VariableValue & _liquid_saturation;
  VariableValue & _liquid_pressure;
  VariableGradient & _grad_liquid_pressure;
  VariableValue & _gas_pressure;
  VariableGradient & _grad_gas_pressure;
  VariableValue & _temperature;

 /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;

};

#endif //FLUIDSTATEMATERAIL_H
