#ifndef GRIZZLYAPP_H
#define GRIZZLYAPP_H

#include "MooseApp.h"

class GrizzlyApp;

template<>
InputParameters validParams<GrizzlyApp>();

class GrizzlyApp : public MooseApp
{
public:
  GrizzlyApp(InputParameters parameters);
  virtual ~GrizzlyApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* GRIZZLYAPP_H */
