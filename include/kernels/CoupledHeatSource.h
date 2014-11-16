#ifndef COUPLEDHEATSOURCE_H
#define COUPLEDHEATSOURCE_H

#include "Kernel.h"

//Forward Declarations
class CoupledHeatSource;

template<>
InputParameters validParams<CoupledHeatSource>();

class CoupledHeatSource : public Kernel
{
public:
  CoupledHeatSource(const std::string & name, InputParameters parameters);
    
protected:
  virtual Real computeQpResidual();
    
  virtual Real computeQpJacobian();
  
private:
  const VariableValue & _powerDensity;
  const Real _scaledPower;
};

#endif
