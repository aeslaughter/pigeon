#ifndef TIGGERAPP_H
#define TIGGERAPP_H

#include "MooseApp.h"

class TiggerApp;

template<>
InputParameters validParams<TiggerApp>();

class TiggerApp : public MooseApp
{
public:
  TiggerApp(const std::string & name, InputParameters parameters);
  virtual ~TiggerApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* TIGGERAPP_H */
