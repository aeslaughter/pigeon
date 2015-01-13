#ifndef POROUSMATERIAL
#define POROUSMATERIAL

#include "Material.h"

class PorousMaterial;

template<>
InputParameters validParams<Material>();

class PorousMaterial : public Material
{
public:
	PorousMaterial(const std::string & name, InputParameters parameters);

protected:
	virtual void computeQpProperties();

private:
	Real _porosity;
	Real _solid_density;
    Real _fluid_density;
	Real _solid_heat_capacity;
	Real _fluid_heat_capacity;
    Real _solid_thermal_conductivity;
    Real _fluid_thermal_conductivity;
    Real _fluid_compressibility;
    Real _permeability;
    Real _fluid_viscosity;

    VariableGradient & _grad_pressure;

    MaterialProperty<Real> & _bulk_thermal_conductivity;
    MaterialProperty<Real> & _bulk_specific_heat;
    MaterialProperty<Real> & _thermal_advection_coeff;
    MaterialProperty<Real> & _fluid_transient_coeff;
    MaterialProperty<Real> & _hydraulic_conductivity;
    MaterialProperty<RealGradient> & _darcy_velocity;
};
#endif //POROUSMATERIAL