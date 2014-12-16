#ifndef PRESSURETIMEDERIVATIVE
#define PRESSURETIMEDERIVATIVE

#include "TimeDerivative.h"
#include "PorousMaterial.h"

class PressureTimeDerivative;

template<>
InputParameters validParams<PressureTimeDerivative>();

class PressureTimeDerivative : public TimeDerivative
{
public:
	PressureTimeDerivative(const std::string & name, InputParameters parameters);

protected:
	virtual Real computeQpResidual();
	virtual Real computeQpJacobian();

private:
	MaterialProperty<Real> & _fluid_transient_coeff;
};
#endif //PRESSURETIMEDERIVATIVE