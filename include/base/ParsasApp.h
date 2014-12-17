#ifndef PARSASAPP_H
#define PARSASAPP_H

#include "MooseApp.h"

class ParsasApp;

template<>
InputParameters validParams<ParsasApp>();

class ParsasApp : public MooseApp
{
public:
  ParsasApp(const std::string & name, InputParameters parameters);
  virtual ~ParsasApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* PARSASAPP_H */
