#include "GrainRadiusAux.h"
#include "Material.h"

template<>
InputParameters validParams<GrainRadiusAux>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredCoupledVar("temp", "Coupled Temperature");
  return params;
}

GrainRadiusAux::GrainRadiusAux(const InputParameters & parameters) : AuxKernel(parameters),

  conversion_factor(1.56/2.),  // grain radius to linear intercept ratio acc. to M.I. Mendelson (J. Am. Ceram. Soc. 52, p.8, 1969)
  gas_constant(8.314),         // perfect gas constant [J/K]

  _temp(coupledValue("temp"))

{}

Real
GrainRadiusAux::computeValue()
{

  /* Grain growth model from J.B. Ainscough et al. (JNM 49, p.117, 1973) */

  Real linear_intercept = _u_old[_qp] / conversion_factor * 1.e+06;  // linear intercept grain size [micron]

  Real dt_h = _dt / 3600.;  // time step in h

  const Real kinetic_coefficient ( 5.24e+07 * exp( -2.67e+05 / ( gas_constant * _temp[_qp] ) ) );
  const Real limit_intercept ( 2.23e+03 * exp( -7.62e+03 / _temp[_qp] ) );

  // solve grain growth equation incrementally

  Real growth_rate ( kinetic_coefficient * ( 1./linear_intercept - 1./limit_intercept ) );  // grain growth rate [micron/h]
  if ( growth_rate < 0. )  growth_rate = 0.;

  const Real dlinear_intercept ( growth_rate * dt_h );

  const Real linear_intercept_old = linear_intercept;

  linear_intercept += dlinear_intercept;
  if ( linear_intercept > limit_intercept && limit_intercept >= linear_intercept_old )
       linear_intercept = limit_intercept;

  // return grain radius in m

  return linear_intercept * conversion_factor * 1.e-06;

}
