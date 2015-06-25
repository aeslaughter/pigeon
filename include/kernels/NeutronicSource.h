#ifndef NEUTRONICSOURCE_H 
#define NEUTRONICSOURCE_H

#include "EigenKernel.h"

//Forward Declarations 
class NeutronicSource;

template<>
InputParameters validParams<NeutronicSource>();

class NeutronicSource : public EigenKernel 
{
public:
  NeutronicSource(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpResidual(); 
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _nu_sigma_f_xs; };
#endif //NEUTRONICSOURCE_H
