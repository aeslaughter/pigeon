#ifndef QUOLLAPP_H
#define QUOLLAPP_H

#include "MooseApp.h"

class QuollApp;

template<>
InputParameters validParams<QuollApp>();

class QuollApp : public MooseApp
{
public:
  QuollApp(const std::string & name, InputParameters parameters);
  virtual ~QuollApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* QUOLLAPP_H */
