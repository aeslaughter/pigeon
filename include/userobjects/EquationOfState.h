/****************************************************************/
/* Equation of state (EOS) base class for multiphase flow in    */
/* porous media                                                 */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef EQUATIONOFSTATE_H
#define EQUATIONOFSTATE_H

#include "GeneralUserObject.h"

class EquationOfState;


template<>
InputParameters validParams<EquationOfState>();

/**
 * Base class for the equation of state (EOS) for multiphase flow in porous media.
 */
class EquationOfState : public GeneralUserObject
{
 public:
  EquationOfState(const std::string & name, InputParameters parameters);

  void initialize();
  void execute();
  void finalize();

  /**
   * Each derived EOS class must overwrite the following fluid properties as 
   * functions of pressure and temperature.
   * density (kg/m^3)
   * viscosity (Pa. s)
   *
   * @param pressure is the gas-phase pore pressure (Pa)
   * @param temperature is the fluid temperature (C)
   *
   */
  virtual Real density(Real pressure, Real temperature) const = 0;
  virtual Real viscosity(Real pressure, Real temperature) const = 0;


};

#endif // EQUATIONOFSTATE_H
