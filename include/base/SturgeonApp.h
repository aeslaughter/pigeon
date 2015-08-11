#ifndef STURGEONAPP_H
#define STURGEONAPP_H

#include "MooseApp.h"

class SturgeonApp;

template<>
InputParameters validParams<SturgeonApp>();

class SturgeonApp : public MooseApp
{
public:
  SturgeonApp(InputParameters parameters);
  virtual ~SturgeonApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* STURGEONAPP_H */
