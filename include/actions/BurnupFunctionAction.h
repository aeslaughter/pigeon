#ifndef BURNUPFUNCTIONACTION_H
#define BURNUPFUNCTIONACTION_H

#include "AddFunctionAction.h"

class BurnupFunctionAction : public AddFunctionAction
{
public:
  BurnupFunctionAction(const std::string & name, InputParameters params);
};

template<>
InputParameters validParams<BurnupFunctionAction>();


#endif
