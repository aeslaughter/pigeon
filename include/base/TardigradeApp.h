#ifndef TARDIGRADEAPP_H
#define TARDIGRADEAPP_H

#include "MooseApp.h"

class TardigradeApp;

template<>
InputParameters validParams<TardigradeApp>();

class TardigradeApp : public MooseApp
{
public:
  TardigradeApp(const std::string & name, InputParameters parameters);
  virtual ~TardigradeApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* TARDIGRADEAPP_H */
