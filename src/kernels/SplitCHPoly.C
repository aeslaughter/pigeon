#include "SplitCHPoly.h"

template<>
InputParameters validParams<SplitCHPoly>()
{
  InputParameters params = validParams<SplitCHCRes>();
  params.addRequiredParam<unsigned int>("order", "Polynomial order");
  params.addParam<std::string>("c_eq_name", "cv_eq", "Material name storing the equilibrium concentration");
  return params;
}

SplitCHPoly::SplitCHPoly(const std::string & name, InputParameters parameters) :
    SplitCHCRes(name, parameters),
    _c_eq_name(getParam<std::string>("c_eq_name")),
    _cv_eq(getMaterialProperty<Real>(_c_eq_name)),
    _W(getMaterialProperty<Real>("barr_height")),
    _order(getParam<unsigned int>("order"))
{
}

Real
SplitCHPoly::computeDFDC(PFFunctionType type)
{
  const Real c = _u[_qp];
  const Real c_eq = _cv_eq[_qp];
  const Real W = _W[_qp];

  switch (type)
  {
    case Residual:
    {
      Real dfdc = 0.0;

      if (_order == 4)
        dfdc = 16.0 * W * (
                   4.0 * c*c*c - 6.0 * c*c
                 + (4.0 * c_eq - 4.0 * c_eq*c_eq + 2.0) * c
                 - 2.0 * c_eq + 2.0 * c_eq*c_eq
               );
      else if (_order == 6)
        dfdc = 96.0 * W * (
                   8.0 * c*c*c*c*c - 20.0 * c*c*c*c
                 + (18.0 - 8.0 * c_eq*c_eq + 8.0 * c_eq) * c*c*c
                 + (-12.0 * c_eq - 7.0 + 12.0 * c_eq*c_eq) * c*c
                 + (1.0 + 6.0 * c_eq - 6.0 * c_eq*c_eq) *c
                 + c_eq*c_eq - c_eq
               );
      else if (_order == 8)
        dfdc = 128.0 * W * (
                   48.0 * c*c*c*c*c*c*c
                 + (-96.0 * c_eq - 168.0) * c*c*c*c*c*c
                 + (48.0 * c_eq*c_eq + 240.0 + 288.0 * c_eq) * c*c*c*c*c
                 + (-120.0 * c_eq*c_eq - 180.0 - 360.0 * c_eq) * c*c*c*c
                 + (240.0 * c_eq + 120.0 * c_eq*c_eq + 75.0) * c*c*c
                 + (-60.0 * c_eq*c_eq - 90.0 * c_eq - 33.0/2.0) * c*c
                 + (18.0 * c_eq + 3.0/2.0 + 15.0 * c_eq*c_eq) * c
                 - 3.0/2.0 * c_eq - 3.0/2.0 * c_eq*c_eq
               );

      // return Residual value
      return dfdc;
    }

    case Jacobian:
    {
      Real d2fdc2 = 0.0;

      if (_order == 4)
        d2fdc2 = W *32.0 * (1.0 + 2.0 * c_eq - 2.0 * c_eq*c_eq - 6.0 * c + 6.0 * c*c);
      else if (_order == 6)
        d2fdc2 = 96.0 * W * (
                     40.0 * c*c*c*c - 80.0 * c*c*c
                   + (54.0 + 24.0 * c_eq - 24.0 * c_eq*c_eq) * c*c
                   + (24.0 * c_eq*c_eq - 24.0 * c_eq - 14.0) * c
                   + 1.0 + 6.0 * c_eq - 6.0 * c_eq*c_eq
                 );
      else if (_order == 8)
        d2fdc2 = 128.0 * W * (
                     336.0 * c*c*c*c*c*c + (-576.0*c_eq-1008.0) * c*c*c*c*c
                   + (1200.0 + 240.0 * c_eq*c_eq + 1440.0 * c_eq) * c*c*c*c
                   + (-1440.0 * c_eq - 480.0 * c_eq*c_eq - 720.0) * c*c*c
                   + (360.0 * c_eq*c_eq + 225.0 + 720.0 * c_eq) * c*c
                   + (-180.0 * c_eq - 120.0 * c_eq*c_eq - 33.0) * c
                   + 18.0 * c_eq + 3.0/2.0 + 15.0 * c_eq*c_eq
                 );

      // return Jacobian value
      return _phi[_j][_qp] * d2fdc2;
    }
  }

  mooseError("Invalid type passed in");
}
