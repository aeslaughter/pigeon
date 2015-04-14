#ifndef TREXAPP_H
#define TREXAPP_H

#include "MooseApp.h"

class TrexApp;

template<>
InputParameters validParams<TrexApp>();

class TrexApp : public MooseApp
{
public:
  TrexApp(const std::string & name, InputParameters parameters);
  virtual ~TrexApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* TREXAPP_H */
