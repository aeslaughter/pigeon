/*************************************************************************
*
*  Welcome to HYRAX!
*  Andrea M. Jokisaari
*  CASL/MOOSE
*
*  7 December 2012
*
*************************************************************************/

#ifndef NUCLEATIONLOCATIONUSEROBJECT_H
#define NUCLEATIONLOCATIONUSEROBJECT_H

#include "ElementUserObject.h"
#include "MooseRandom.h"
#include "Nucleus.h"

//Forward declarations
class NucleationLocationUserObject;

template<>
InputParameters validParams<NucleationLocationUserObject>();

class NucleationLocationUserObject : public ElementUserObject
{
public:
  NucleationLocationUserObject (const std::string & name, InputParameters parameters);
  virtual ~NucleationLocationUserObject() {}

  virtual void initialize();
  virtual void execute();
  virtual void destroy() {}
  virtual void finalize();
  virtual void threadJoin(const UserObject &a) {}

  bool elementWasHit(const Elem * elem) const;

  const std::vector<Nucleus> & getNuclei() const { return _nuclei; }

protected:
private:

  MooseMesh & _mesh;
  VariableValue & _coupled_probability;

  Real _dwell_time;
  int _num_orientations;

  int _counter;

   /// The index of the phase orientation generator (we will use a high index that isn't used by the node generators)
  const unsigned int _phase_gen_index;

  //local data that's concatenated onto the "global" array at the end of each step
  std::vector<Nucleus> _local_nucleus;

  //global data
  std::vector<Nucleus> _nuclei;

  // The Moose stateful random number generator
  MooseRandom _mrand;

};

#endif //NUCLEATIONLOCATIONUSEROBJECT_H