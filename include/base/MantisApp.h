#ifndef MANTISAPP_H
#define MANTISAPP_H

#include "MooseApp.h"

class MantisApp;

template<>
InputParameters validParams<MantisApp>();

class MantisApp : public MooseApp
{
public:
  MantisApp(InputParameters parameters);
  virtual ~MantisApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* MANTISAPP_H */
