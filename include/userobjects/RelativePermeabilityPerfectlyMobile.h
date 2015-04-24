/****************************************************************/
/* Perfectly mobile relative permeability for multiphase flow   */
/* in porous media.                                             */
/*                                                              */
/* Relative permeability is unity for all saturations.          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef RELATIVEPERMEABILITYPERFECTLYMOBILE_H
#define RELATIVEPERMEABILITYPERFECTLYMOBILE_H

#include "RelativePermeability.h"

class RelativePermeabilityPerfectlyMobile;


template<>
InputParameters validParams<RelativePermeabilityPerfectlyMobile>();

class RelativePermeabilityPerfectlyMobile : public RelativePermeability
{
 public:
  RelativePermeabilityPerfectlyMobile(const std::string & name, InputParameters parameters);

  /**
   * Relative permeability as a function of liquid saturation
   * @param sat_liq is liquid phase saturation
   */
  Real relativePermLiq(Real sat_liq) const;
  Real relativePermGas(Real sat_liq) const;

 protected:

};

#endif // RELATIVEPERMEABILITYPERFECTLYMOBILE_H
