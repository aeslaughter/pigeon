#ifndef PRESSUREDARCY
#define PRESSUREDARCY

#include "Diffusion.h"
#include "PorousMaterial.h"

class PressureDarcy;

template<>
InputParameters validParams<PressureDarcy>();

class PressureDarcy : public Diffusion
{
public:
	PressureDarcy(const std::string & name, InputParameters parameters);

protected:
	virtual Real computeQpResidual();
	virtual Real computeQpJacobian();

	MaterialProperty<Real> & _hydraulic_conductivity;
};
#endif //PRESSUREDARCY