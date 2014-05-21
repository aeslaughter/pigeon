#ifndef LELEAPP_H
#define LELEAPP_H

#include "MooseApp.h"

class LeleApp;

template<>
InputParameters validParams<LeleApp>();

class LeleApp : public MooseApp
{
public:
  LeleApp(const std::string & name, InputParameters parameters);
  virtual ~LeleApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* LELEAPP_H */
