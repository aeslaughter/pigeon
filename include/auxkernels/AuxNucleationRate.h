/*************************************************************************
*
*  Welcome to HYRAX!
*  Andrea M. Jokisaari
*  CASL/MOOSE
*
*  15 December 2011
*
*************************************************************************/

#ifndef AUXNUCLEATIONRATE_H
#define AUXNUCLEATIONRATE_H

#include "AuxKernel.h"

class AuxNucleationRate;

template<>
InputParameters validParams<AuxNucleationRate>();

/**
 *  AuxNucleationRate handles the nucleation rate (j_star) calculation in the concurrent
 *  nucleation and growth algorithm first proposed by J.P. Simmons (2000).
 */

class AuxNucleationRate : public AuxKernel
{
public:
  AuxNucleationRate(const std::string & name, InputParameters parameters);

protected:
  /**
   * computeValue()
   * @return returns the nucleation rate (element average value), j_star.
   * Handles adaptivity and different mesh dimensions.
   * j_star = Kn1 * exp(-1*Kn2 / supersaturation)
   */

  virtual Real computeValue();

private:

  VariableValue & _coupled_supersaturation; ///< concentration supersaturation
  //Real _Kn1;  				    ///< First nucleation rate value
  Real _Kn2;				    ///< Second nucleation rate value
  Real _Z;                                  // Zeldovich non-equilibrium factor
  //Real _N;                                  // # of atoms in phase field cell
  Real _beta_star;                          // 1/characteristic nucleation time
  Real _linear_density;                     // linear atomic density of material

};

#endif //AUXNUCLEATIONRATE_H
