#ifndef TURKEYAPP_H
#define TURKEYAPP_H

#include "MooseApp.h"

class TurkeyApp;

template<>
InputParameters validParams<TurkeyApp>();

class TurkeyApp : public MooseApp
{
public:
  TurkeyApp(InputParameters parameters);
  virtual ~TurkeyApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* TURKEYAPP_H */
