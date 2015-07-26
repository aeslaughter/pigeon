#ifndef BANANAAPP_H
#define BANANAAPP_H

#include "MooseApp.h"

class BananaApp;

template<>
InputParameters validParams<BananaApp>();

class BananaApp : public MooseApp
{
public:
  BananaApp(const std::string & name, InputParameters parameters);
  virtual ~BananaApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* BANANAAPP_H */
