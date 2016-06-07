#ifndef PIGEONAPP_H
#define PIGEONAPP_H

#include "MooseApp.h"

class PigeonApp;

template<>
InputParameters validParams<PigeonApp>();

class PigeonApp : public MooseApp
{
public:
  PigeonApp(InputParameters parameters);
  virtual ~PigeonApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* PIGEONAPP_H */
