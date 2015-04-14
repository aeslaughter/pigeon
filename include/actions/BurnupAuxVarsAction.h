#ifndef BURNUPAUXVARSACTION_H
#define BURNUPAUXVARSACTION_H

#include "Action.h"

class BurnupAuxVarsAction : public Action
{
public:
  BurnupAuxVarsAction(const std::string & name, InputParameters params);
  virtual ~BurnupAuxVarsAction(){}
  virtual void act();

private:
  const std::string _order;
  const std::string _family;
  const std::string _N235;
  const std::string _N238;
  const std::string _N239;
  const std::string _N240;
  const std::string _N241;
  const std::string _N242;
  const std::string _RPF;
};

template<>
InputParameters validParams<BurnupAuxVarsAction>();


#endif
