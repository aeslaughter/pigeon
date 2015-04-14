#ifndef ELEMENTINTEGRALFISGASRELEASEDSIFGRS_H
#define ELEMENTINTEGRALFISGASRELEASEDSIFGRS_H

#include "ElementIntegralVariablePostprocessor.h"

//Forward Declarations
class ElementIntegralFisGasReleasedSifgrs;

template<>
InputParameters validParams<ElementIntegralFisGasReleasedSifgrs>();

/**
  * This postprocessor computes the total amount of released fission gas in the fuel rod. It is to be used in combination with the Sifgrs model.
 */
class ElementIntegralFisGasReleasedSifgrs : public ElementIntegralVariablePostprocessor
{
public:
  ElementIntegralFisGasReleasedSifgrs(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();

  MaterialProperty<Real> & _gas_rel_3;
};

#endif
