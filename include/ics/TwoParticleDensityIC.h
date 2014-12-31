#ifndef TWOPARTICLEDENSITYIC_H
#define TWOPARTICLEDENSITYIC_H

#include "Kernel.h"
#include "InitialCondition.h"

// System includes
#include <string>

// Forward Declarations
class TwoParticleDensityIC;

template<>
InputParameters validParams<TwoParticleDensityIC>();

/**
 * TwoParticleGrainsIC creates a grain structure with 2 circle grains and one matrix grain
*/
class TwoParticleDensityIC : public InitialCondition
{
public:
  /**
   * Constructor
   *
   * @param name The name given to the initial condition in the input file.
   * @param parameters The parameters object holding data for the class to use.
   * @param var_name The variable this InitialCondtion is supposed to provide values for.
   */
  TwoParticleDensityIC(const std::string & name,
                InputParameters parameters);

  /**
   * The value of the variable at a point.
   *
   * This must be overriden by derived classes.
   */
  virtual Real value(const Point & p);

protected:
  MooseMesh & _mesh;
  /// A reference to the nonlinear system
  NonlinearSystem & _nl;
 
  Real _minvalue;
  Real _maxvalue;
  Real _rangevalue;
  
  unsigned int _op_num;
  std::vector<Real> _radius;

  Point _bottom_left;
  Point _top_right;
  Point _rangedomain;
};

#endif //TWOPARTICLEDENSITYIC_H
