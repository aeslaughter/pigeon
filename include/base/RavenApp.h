#ifndef RAVENAPP_H
#define RAVENAPP_H

#include "MooseApp.h"

class RavenApp;

template<>
InputParameters validParams<RavenApp>();

class RavenApp : public MooseApp
{
public:
  RavenApp(InputParameters parameters);
  virtual ~RavenApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* RAVENAPP_H */
