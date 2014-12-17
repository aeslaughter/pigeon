#ifndef THERMALADVECTION
#define THERMALADVECTION

#include "Kernel.h"
#include "Material.h"

class ThermalAdvection;

template<>
InputParameters validParams<ThermalAdvection>();

class ThermalAdvection : public Kernel
{
public:
	ThermalAdvection(const std::string & name, InputParameters parameters);

protected:
	virtual Real computeQpResidual();
	virtual Real computeQpJacobian();

private:
	MaterialProperty<Real> & _thermal_advection_coeff;
	VariableGradient & _grad_pressure;
};
#endif //THERMALADVECTION