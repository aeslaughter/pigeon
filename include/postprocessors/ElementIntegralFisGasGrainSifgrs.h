#ifndef ELEMENTINTEGRALFISGASGRAINSIFGRS_H
#define ELEMENTINTEGRALFISGASGRAINSIFGRS_H

#include "ElementIntegralVariablePostprocessor.h"

//Forward Declarations
class ElementIntegralFisGasGrainSifgrs;

template<>
InputParameters validParams<ElementIntegralFisGasGrainSifgrs>();

/**
 * This postprocessor computes the total amount of intragranular fission gas in the fuel rod. It is to be used in combination with the Sifgrs model.
 */
class ElementIntegralFisGasGrainSifgrs : public ElementIntegralVariablePostprocessor
{
public:
  ElementIntegralFisGasGrainSifgrs(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();

  MaterialProperty<Real> & _gas_grn_3;
};

#endif
