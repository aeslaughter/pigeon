#ifndef ELEMENTINTEGRALFISGASGENERATEDSIFGRS_H
#define ELEMENTINTEGRALFISGASGENERATEDSIFGRS_H

#include "ElementIntegralVariablePostprocessor.h"

//Forward Declarations
class ElementIntegralFisGasGeneratedSifgrs;

template<>
InputParameters validParams<ElementIntegralFisGasGeneratedSifgrs>();

/**
 * This postprocessor computes the total amount of generated fission gas in the fuel rod. It is to be used in combination with the Sifgrs model.
 */
class ElementIntegralFisGasGeneratedSifgrs : public ElementIntegralVariablePostprocessor
{
public:
  ElementIntegralFisGasGeneratedSifgrs(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();

  MaterialProperty<Real> & _gas_gen_3;
};

#endif
