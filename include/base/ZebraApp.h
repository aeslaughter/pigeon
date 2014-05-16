#ifndef ZEBRAAPP_H
#define ZEBRAAPP_H

#include "MooseApp.h"

class ZebraApp;

template<>
InputParameters validParams<ZebraApp>();

class ZebraApp : public MooseApp
{
public:
  ZebraApp(const std::string & name, InputParameters parameters);
  virtual ~ZebraApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* ZEBRAAPP_H */
