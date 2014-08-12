#ifndef LOONAPP_H
#define LOONAPP_H

#include "MooseApp.h"

class LoonApp;

template<>
InputParameters validParams<LoonApp>();

class LoonApp : public MooseApp
{
public:
  LoonApp(const std::string & name, InputParameters parameters);
  virtual ~LoonApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* LOONAPP_H */
