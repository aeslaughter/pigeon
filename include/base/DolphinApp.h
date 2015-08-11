#ifndef DOLPHINAPP_H
#define DOLPHINAPP_H

#include "MooseApp.h"

class DolphinApp;

template<>
InputParameters validParams<DolphinApp>();

class DolphinApp : public MooseApp
{
public:
  DolphinApp(InputParameters parameters);
  virtual ~DolphinApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* DOLPHINAPP_H */
