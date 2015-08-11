#ifndef SPLITCHPOLY_H
#define SPLITCHPOLY_H

#include "SplitCHCRes.h"

//Forward Declarations
class SplitCHPoly;

template<>
InputParameters validParams<SplitCHPoly>();

/**
 * SplitCHPoly is the split operator version of CHPoly. It contains a polynomial
 * double well free energy functionals for simulating void nucleation and growth.
 * It uses polynomial orders 4, 6 and 8 with zeros at cv_eq and 1-cv_eq. It must be
 * used in conjunction with CHinterface to represent the entire Cahn-Hilliard
 * equation.
 */
class SplitCHPoly : public SplitCHCRes
{
public:
  SplitCHPoly(const InputParameters & parameters);

protected:
  virtual Real computeDFDC(PFFunctionType type);

private:
  std::string _c_eq_name;
  MaterialProperty<Real> & _cv_eq;
  MaterialProperty<Real> & _W;

  unsigned int _order;
};

#endif //SPLITCHPOLY_H
