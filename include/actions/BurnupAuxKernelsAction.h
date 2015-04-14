#ifndef BURNUPAUXKERNELSACTION_H
#define BURNUPAUXKERNELSACTION_H

#include "Action.h"

class BurnupAuxKernelsAction : public Action
{
public:
  BurnupAuxKernelsAction(const std::string & name, InputParameters params);

  virtual void act();

private:
  std::vector<SubdomainName> _blocks;
  const Real _energy_per_fission;
  const std::string _N235;
  const std::string _N238;
  const std::string _N239;
  const std::string _N240;
  const std::string _N241;
  const std::string _N242;
  const std::string _RPF;
};

template<>
InputParameters validParams<BurnupAuxKernelsAction>();


#endif
