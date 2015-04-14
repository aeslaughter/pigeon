#ifndef NEUTRONHEATSOURCE_H
#define NEUTRONHEATSOURCE_H

#include "Kernel.h"
#include "Function.h"

//Forward Declarations
class BurnupFunction;

class NeutronHeatSource : public Kernel
{
public:

  NeutronHeatSource(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpResidual();

private:
  const Real _energy_per_fission;
  const std::string _units;
  const bool _has_fission_rate;
  const VariableValue & _fission_rate;

  const bool _has_max_fission_rate;
  const VariableValue & _max_fission_rate;
  const Real & _decay_heat_function;

  BurnupFunction * const _burnup_function;

  Function * const _rod_ave_lin_pow;
  Function * const _axial_profile;
  const bool _has_diameter;
  const Real _outer_diameter;
  const Real _inner_diameter;
  const bool _has_area;
  const Real _area;
  const Real _fraction;

};

template<>
InputParameters validParams<NeutronHeatSource>();

#endif
