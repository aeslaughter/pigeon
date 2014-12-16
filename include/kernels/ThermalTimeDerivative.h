#ifndef THERMALTIMEDERIVATIVE
#define THERMALTIMEDERIVATIVE

#include "TimeDerivative.h"
#include "PorousMaterial.h"

class ThermalTimeDerivative;

template<>
InputParameters validParams<ThermalTimeDerivative>();

class ThermalTimeDerivative : public TimeDerivative
{
public:
	ThermalTimeDerivative(const std::string & name, InputParameters parameters);

protected:
	virtual Real computeQpResidual();
	virtual Real computeQpJacobian();

private:
	MaterialProperty<Real> & _bulk_specific_heat;
};
#endif //THERMALTIMEDERIVATIVE 