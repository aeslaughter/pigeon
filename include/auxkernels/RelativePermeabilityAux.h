/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate relative permeability for      */
/*  multiphase flow in porous media using the relative          */
/* permeability relationship defined in the relative            */
/*  permeability UserObject.                                    */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef RELATIVEPERMEABILITYAUX_H
#define RELATIVEPERMEABILITYAUX_H

#include "AuxKernel.h"
#include "RelativePermeability.h"

class RelativePermeabilityAux;

template<>
InputParameters validParams<RelativePermeabilityAux>();

class RelativePermeabilityAux : public AuxKernel
{
public:

  RelativePermeabilityAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
    * This is the member reference that will hold the User Object
    * value for relative permeability.
    */
  const RelativePermeability & _relative_permeabilityUO;
  VariableValue & _liquid_saturation;
  const MooseEnum & fluid_phase_enum;
};

#endif //RELATIVEPERMEABILITYAUX_H
