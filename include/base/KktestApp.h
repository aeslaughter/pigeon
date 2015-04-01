#ifndef KKTESTAPP_H
#define KKTESTAPP_H

#include "MooseApp.h"

class KktestApp;

template<>
InputParameters validParams<KktestApp>();

class KktestApp : public MooseApp
{
public:
  KktestApp(const std::string & name, InputParameters parameters);
  virtual ~KktestApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* KKTESTAPP_H */
