#ifndef THERMALCONDUCTION
#define THERMALCONDUCTION

#include "Diffusion.h"
#include "Material.h"

class ThermalConduction;

template<>
InputParameters validParams<ThermalConduction>();

class ThermalConduction : public Diffusion
{
public:
	ThermalConduction(const std::string & name, InputParameters parameters);

protected:
	virtual Real computeQpResidual();
	virtual Real computeQpJacobian();

private:
	MaterialProperty<Real> & _bulk_thermal_conductivity;
};
#endif //THERMALCONDUCTION