#include "CreepPyC.h"

#include "Element.h"

#include "SymmIsotropicElasticityTensor.h"

template<>
InputParameters validParams<CreepPyC>()
{
  InputParameters params = validParams<SolidModel>();

  //  Material parameters
  params.addRequiredParam<Real>("density", "Initial fuel density");
  params.addRequiredCoupledVar("flux", "Coupled flux");

  return params;
}


CreepPyC::CreepPyC( const std::string & name,
                    InputParameters parameters )
  :SolidModel( name, parameters ),
   _densitygcc(getParam<Real>("density") * 0.001), // kg/m3 -> g/cc
   _fast_neutron_flux(coupledValue("flux")),
   _nu( 0.5 ),
   _K( 2.715e-29 ),
   _creep_strain(declareProperty<SymmTensor>("creep_strain")),
   _creep_strain_old(declarePropertyOld<SymmTensor>("creep_strain"))
{
  if (!isCoupled("temp"))
  {
    mooseError("Must supply temperature in CreepPyC");
  }
}

void
CreepPyC::computeNuAndK()
{
  _nu = 0.5;
  const Real MirrCreep(2);
  const Real tempC( _temperature[_qp] - 273.15 );
  const Real K0( 1.996e-29 - 4.415e-32*tempC + 3.6544e-35*tempC*tempC );
  _K = K0 * (1 + 2.38*(1.9 - _densitygcc)) * MirrCreep;
}

void
CreepPyC::computeStressExplicit()
{

  // CEA creep model taken from Petti 2002, 2005 report
  // See also "Development of Improved Models and Designs for Coated-Particle Gas Reactor Fuels," INEEL/EXT-05-02615, 2004, and
  // "A review of TRISO fuel performance models," Powers and Wirth, JNM, 405, p. 74-82.

  // creep rate = K * (sigma1 - nu * (sigma2 + sigma3)) * flux

  // Given the stretching, compute the stress increment and add it to the old stress. Also update the creep strain
  // stress = stressOld + stressIncrement
  // creep_strain = creep_strainOld + creep_strainIncrement

  if(_t_step == 0) return;

  // compute trial stress
  SymmTensor stress(_stress_old);

  // The approach is to first compute the principal stresses and use the principal
  // stress tensor in all of the creep calculations.  The resulting incremental
  // creep tensor must be rotated back to the global frame as a final step.

  // Get the principal stresses
  ColumnMajorMatrix principal_stress(3, 1);
  ColumnMajorMatrix eigen_vectors(3, 3);
  stress.columnMajorMatrix().eigen( principal_stress, eigen_vectors );

  // extract principal stresses
  const Real p1( principal_stress(0,0) );
  const Real p2( principal_stress(1,0) );
  const Real p3( principal_stress(2,0) );

  computeNuAndK();

  // compute scalar creep strain increment
  // The 1e-6 is to convert the stresses to MPa
  Real del_p1 = _dt * _K * 1e-6 * ( p1 - _nu * ( p2 + p3 ) ) * _fast_neutron_flux[_qp];

  Real del_p2 = _dt * _K * 1e-6 * ( p2 - _nu * ( p3 + p1 ) ) * _fast_neutron_flux[_qp];

  Real del_p3 = _dt * _K * 1e-6 * ( p3 - _nu * ( p1 + p2 ) ) * _fast_neutron_flux[_qp];

  SymmTensor creep_strain_increment( del_p1, del_p2, del_p3, 0, 0, 0 );

  // Rotate creep_strain_increment back to global frame
  SolidMechanics::Element::rotateSymmetricTensor( eigen_vectors, creep_strain_increment, creep_strain_increment );



  _strain_increment -= creep_strain_increment;

  // update stress and creep strain
  // compute stress increment
  _stress[_qp] =  *elasticityTensor() * _strain_increment;

  _stress[_qp] += _stress_old;
  _creep_strain[_qp] = creep_strain_increment;
  _creep_strain[_qp] += _creep_strain_old[_qp];

}

void
CreepPyC::computeStress()
{
  // CEA creep model taken from Petti 2002, 2005 report
  // creep rate = K * (sigma1 - nu * (sigma2 + sigma3)) * flux

  // Given the stretching, compute the stress increment and add it to the old stress. Also update the creep strain
  // stress = stressOld + stressIncrement
  // creep_strain = creep_strainOld + creep_strainIncrement

  if(_t_step == 0) return;

  // compute trial stress
  SymmTensor stress = *elasticityTensor() * _strain_increment;
  stress += _stress_old;

  // The approach is to first compute the principal stresses and use the principal
  // stress tensor in all of the creep calculations.  The resulting incremental
  // creep tensor must be rotated back to the global frame as a final step.

  // Get the principal stresses
  ColumnMajorMatrix principal_stress(3, 1);
  ColumnMajorMatrix eigen_vectors(3, 3);
  stress.columnMajorMatrix().eigen( principal_stress, eigen_vectors );

  // extract principal stresses
  const Real p1( principal_stress(0,0) );
  const Real p2( principal_stress(1,0) );
  const Real p3( principal_stress(2,0) );

  computeNuAndK();

  // The creep strain rate is defined as
  // phi = C0 * f(sigma)
  // where
  //   phi = creep strain rate tensor (represented as 3x1 vector)
  //   C0 represents K and fast neutron flux contributions
  //   sigma = trial_stress_tensor - C * creep_strain_increment_tensor (represented as 3x1 vector)
  //   C = fourth order elasticity tensor (represented as 3x3 tensor)
  //   creep_strain_increment_tensor (csit) = second order tensor (represented as 3x1 vector)
  // So phi = 1/dt * creep_strain_increment_tensor
  // By moving 1/dt*csit to the other side, we define the residual, which must be zero.
  // r = phi - 1/dt*csit = 0
  //   = C0 * f(trial_stress - C * csit) - 1/dt * csit
  //   = C0 * (f(trial_stress) - f(C * csit)) - 1/dt*csit
  //   = dt * C0 * (f(trial_stress) - f(C * csit)) - csit
  // Now let A (3x3 tensor) represent the action of dt * C0 * f(x) and I represent the identity tensor:
  // 0 = A * trial_stress - A * C * csit - I * csit
  //   = A * trial_stress - (A * C + I) * csit
  // So,
  // csit = (A * C + I)^-1 * (A * trial_stress)
  //

  // compute scalar creep strain increment
  // The 1e-6 is to convert the stresses to MPa

  const Real term = _dt * _K * 1e-6 * _fast_neutron_flux[_qp];
  ColumnMajorMatrix A(3,3);
  A(0,0) = A(1,1) = A(2,2) = term;
  A(0,1) = A(1,0) = A(0,2) = A(2,0) = A(1,2) = A(2,1) = -term * _nu;

  ColumnMajorMatrix C(3,3);
  C(0,0) = elasticityTensor()->valueAtIndex(0);
  C(1,0) = C(0,1) = elasticityTensor()->valueAtIndex(1);
  C(2,0) = C(0,2) = elasticityTensor()->valueAtIndex(2);
  C(1,1) = elasticityTensor()->valueAtIndex(6);
  C(1,2) = C(2,1) = elasticityTensor()->valueAtIndex(7);
  C(2,2) = elasticityTensor()->valueAtIndex(11);

  ColumnMajorMatrix ACmI = A*C;
  ACmI(0,0) += 1;
  ACmI(1,1) += 1;
  ACmI(2,2) += 1;

  ColumnMajorMatrix ACmIinv;
  ACmI.inverse(ACmIinv);

  ColumnMajorMatrix rhs(3,1);
  rhs(0,0) = term * ( p1 - _nu * ( p2 + p3 ) );
  rhs(1,0) = term * ( p2 - _nu * ( p3 + p1 ) );
  rhs(2,0) = term * ( p3 - _nu * ( p1 + p2 ) );

  ColumnMajorMatrix csi(3,1);
  csi = ACmIinv * rhs;

  SymmTensor creep_strain_increment( csi(0,0), csi(1,0), csi(2,0), 0, 0, 0 );

  // Rotate creep_strain_increment back to global frame
  SolidMechanics::Element::rotateSymmetricTensor( eigen_vectors, creep_strain_increment, creep_strain_increment );



  _strain_increment -= creep_strain_increment;

  // update stress and creep strain
  // compute stress increment
  _stress[_qp] =  *elasticityTensor() * _strain_increment;

  _stress[_qp] += _stress_old;
  _creep_strain[_qp] = creep_strain_increment;
  _creep_strain[_qp] += _creep_strain_old[_qp];

}
