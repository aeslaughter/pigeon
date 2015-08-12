#ifndef ANTEATERAPP_H
#define ANTEATERAPP_H

#include "MooseApp.h"

class AnteaterApp;

template<>
InputParameters validParams<AnteaterApp>();

class AnteaterApp : public MooseApp
{
public:
  AnteaterApp(InputParameters parameters);
  virtual ~AnteaterApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* ANTEATERAPP_H */
