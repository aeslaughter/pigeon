/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef PLATE_H
#define PLATE_H

#include "Material.h"

// Forward Declarations
class Plate;

template<>
InputParameters validParams<Plate>();

// Plate properties.
class Plate : public Material
{

public:
  Plate(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

private:

  const Real & _l;
  MaterialProperty<Real> & _thickness;
  const Real & _rho;
  MaterialProperty<Real> & _density;
  const Real & _alpha;
  MaterialProperty<Real> & _diffusivity;
  const Real & _c_p;
  MaterialProperty<Real> & _specific_heat;

};

#endif // PLATE_H
