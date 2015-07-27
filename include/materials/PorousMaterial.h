/****************************************************************/
/* Porous material for multiphase flow                          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef POROUSMATERIAL_H
#define POROUSMATERIAL_H

#include "Material.h"
#include "RelativePermeability.h"
#include "CapillaryPressure.h"

//Forward Declarations
class PorousMaterial;

template<>
InputParameters validParams<PorousMaterial>();

/**
 * General material class for multiphase flow through porous media.
 */
class PorousMaterial : public Material
{
public:
  PorousMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

private:
  /// Rock density specified in input file
  Real _material_density;
  /// Porosity specified in input file
  Real _material_porosity;
  /// Permeability tensor specified in input file
  RealTensorValue _material_permeability;
  /// Gravity vector specified in input file
  RealVectorValue _material_gravity;

  /**
   * Member references that will hold the User Object values for
   * the porous material properties.
   */
//  const RelativePermeability & _material_relative_permeability;
//  const CapillaryPressure & _material_capillary_pressure;
  MaterialProperty<Real> & _rock_density;
  MaterialProperty<Real> & _porosity;
  MaterialProperty<RealTensorValue> & _permeability;
  MaterialProperty<RealVectorValue> & _gravity;
//  MaterialProperty<Real> & _gas_relative_permeability;
//  MaterialProperty<Real> & _liquid_relative_permeability;
//  MaterialProperty<Real> & _capillary_pressure;

  /**
   * The member reference of the liquid saturation variable
   */
  VariableValue & _liquid_saturation;

};

#endif //POROUSMATERAIL_H
