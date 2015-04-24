/****************************************************************/
/* Relative permeability base clase for multiphase flow in      */
/* porous media                                                 */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef RELATIVEPERMEABILITY_H
#define RELATIVEPERMEABILITY_H

#include "GeneralUserObject.h"

class RelativePermeability;


template<>
InputParameters validParams<RelativePermeability>();

/**
 * Base class for relative permeability for multiphase flow in porous media.
 */
class RelativePermeability : public GeneralUserObject
{
 public:
  RelativePermeability(const std::string & name, InputParameters parameters);

  void initialize();
  void execute();
  void finalize();

  /**
   * Relative permeability is calculated as a function of saturation.
   * These must be over-ridden in your derived class to provide actual
   * values of relative permeability.
   * @param sat_liq is liquid phase saturation
   */
  virtual Real relativePermLiq(Real sat_liq) const = 0;
  virtual Real relativePermGas(Real sat_liq) const = 0;


};

#endif // RELATIVEPERMEABILITY_H
