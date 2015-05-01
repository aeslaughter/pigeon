/*************************************************************************
*
*  Welcome to HYRAX!
*  Andrea M. Jokisaari
*  CASL/MOOSE
*
*  16 November 2011
*
*************************************************************************/

#ifndef ACBULKCOUPLED_H
#define ACBULKCOUPLED_H

#include "ACBulk.h"

//Forward Declarations
class ACBulkCoupled;

template<>
InputParameters validParams<ACBulkCoupled>();

/**
 * AcBulkCoupled couples the bulk Alan-Cahn equation term with order parameter eta to the
 * conserved field variable term from the Cahn-Hilliard equation (typically concentration).
 * It uses the PFMobilityLandau materials class.
 */
class ACBulkCoupled : public ACBulk
{
public:

  ACBulkCoupled(const std::string & name, InputParameters parameters);

protected:

  /**
   * computeDFDOP()
   * @return returns the partial(bulk free energy/order parameter)
   */
  virtual Real computeDFDOP(PFFunctionType type);

  MaterialProperty<Real> & _a2;  ///< coefficients for the landau polynomial (see Guo, 2008)
  MaterialProperty<Real> & _a3;
  MaterialProperty<Real> & _a4;
  MaterialProperty<Real> & _c2;  ///< energy well position in c-space for the 2nd phase, ish

  VariableValue & _coupled_CH_var;  ///< the Cahn-Hilliard variable (concentration, probably)


private:

};

#endif //ACBULKCOUPLED_H
