#ifndef VULTUREAPP_H
#define VULTUREAPP_H

#include "MooseApp.h"

class VultureApp;

template<>
InputParameters validParams<VultureApp>();

class VultureApp : public MooseApp
{
public:
  VultureApp(const std::string & name, InputParameters parameters);
  virtual ~VultureApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* VULTUREAPP_H */
