#ifndef ANTAPP_H
#define ANTAPP_H

#include "MooseApp.h"

class AntApp;

template<>
InputParameters validParams<AntApp>();

class AntApp : public MooseApp
{
public:
  AntApp(InputParameters parameters);
  virtual ~AntApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* ANTAPP_H */
