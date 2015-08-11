#include "ConcEvolution.h"

template<>
InputParameters validParams<ConcEvolution>()
{
  InputParameters params = validParams<Material>();
  params.addCoupledVar("T", "Temperature variable in Kelvin");
  params.addRequiredCoupledVar("c", "Concentration");
  params.addRequiredParam<Real>("int_width", "The interfacial width of void surface in the lengthscale of the problem");
  params.addParam<Real>("length_scale", 1.0e-9, "defines the base length scale of the problem in m");
  params.addParam<Real>("time_scale", 1.0e-9, "defines the base time scale of the problem");
  params.addRequiredParam<unsigned int>("free_energy_form", "If 0, physical free energy, then polynomial order");
  params.addRequiredParam<Real>("D0", "Diffusivity prefactor for vacancies in m^2/s");
  params.addRequiredParam<Real>("Em", "Vacancy migration energy in eV");
  params.addRequiredParam<Real>("Ef", "Vacancy formation energy in eV");
  params.addRequiredParam<Real>("surface_energy", "Surface energy in J/m2");
  return params;
}

ConcEvolution::ConcEvolution(const InputParameters & parameters) :
    Material(parameters),
    _c(coupledValue("c")),
    _grad_c(coupledGradient("c") ),
    _T(coupledValue("T")),
    _kT(declareProperty<Real>("kT")),
    _M(declareProperty<Real>("M")),
    _Ms(declareProperty<Real>("Ms")),
    _D(declareProperty<Real>("D")),
    _grad_M(declareProperty<RealGradient>("grad_M")),
    _Ef(declareProperty<Real>("Ef")),
    _kappa(declareProperty<Real>("kappa")),
    _c_eq(declareProperty<Real>("c_eq")),
    _W(declareProperty<Real>("barr_height")),
     _Qstar(declareProperty<Real>("Qstar")),
    _int_width(getParam<Real>("int_width")),
    _length_scale(getParam<Real>("length_scale")),
    _time_scale(getParam<Real>("time_scale")),
    _free_energy_form(getParam<unsigned int>("free_energy_form")),
    _D0(getParam<Real>("D0")),
    _Em(getParam<Real>("Em")),
    _Efin(getParam<Real>("Ef")),
    _surface_energy(getParam<Real>("surface_energy")),
    _JtoeV(6.24150974e18), // joule to eV conversion
    _kb(8.617343e-5) // Boltzmann constant in eV/K
{
}

void
ConcEvolution::computeProperties()
{
  // Convert mobility from m^2/s to the length and time scale
  Real D0_c = _D0*_time_scale/(_length_scale*_length_scale);

  // The interfacial width is input in the length scale of the problem, so no conversion is necessary
  Real int_width_c = _int_width;

  for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
  {
    _kT[qp] = _kb * _T[qp];

    _Ef[qp] = _Efin; // eV
    _c_eq[qp] = std::exp(-_Efin/_kT[qp]);
    _D[qp] = D0_c*std::exp(-_Em/_kT[qp]);

    Real KN = 0.0;
    if (_free_energy_form == 4)
      KN = 2.0/3.0;
    else if (_free_energy_form == 6)
      KN = 3.0/16.0 * std::sqrt(3.0) - 9.0/64.0 * std::log((-std::sqrt(2.0) + std::sqrt(3.0)) / (std::sqrt(2.0) + std::sqrt(3.0)));
    else if (_free_energy_form == 8)
      KN = 0.83551;

    // Convert surface energy from J/m2 to eV/length_scale
    Real surf_energy = _surface_energy * _JtoeV * _length_scale*_length_scale;

    if (_free_energy_form == 0)
    {
      Real Mlog = 1.092499; // Constant value calculated for log function
      _kappa[qp] = std::sqrt(2.0) * surf_energy * int_width_c / Mlog;
    }
    else
    {
      _kappa[qp] = surf_energy * int_width_c / KN;
      _W[qp] = surf_energy / (2.0 * int_width_c * KN);
    }

    Real Co = 0.0;
    Real a = _c_eq[qp];
    if (_free_energy_form == 0) // Natural log free energy
    {
      Real c = _c[qp];
      Real tol = 1.0e-9;

      if (c < tol)
        c = tol;
      if (c > 1.0 - tol)
        c = 1.0 - tol;

      _Ms[qp] = _D[qp]/_kT[qp];
      _M[qp] = _Ms[qp] * (c - c*c);
      _grad_M[qp] = _Ms[qp] * _grad_c[qp] * (1.0 - 2.0*c);
    }
    else
    {
      if (_free_energy_form == 4) // 4th order polynomial
      {
        Real C4_2 = 1.0 + 2.0*a - 2.0*a*a;
        Co = std::pow(2.0, 4.0) * C4_2;
      }
      else if (_free_energy_form == 6) // 6th order polynomial
      {
        Real C6_2 = 0.75 + 6.0*a + 4.5*a*a;
        Co = std::pow(2.0, 6.0) * C6_2;
      }
      else if (_free_energy_form == 8) // 8th order polynomial
      {
        Real C8_2 = 3.0/8.0 + 4.5*a + 15.0/4.0*a*a;
        Co = std::pow(2.0, 8.0) * C8_2;
      }

      _M[qp] = KN / Co * (_D[qp] * int_width_c / surf_energy);
      _grad_M[qp] = 0.0;
    }

    _Qstar[qp] = -4.0; // eV
  }
}
