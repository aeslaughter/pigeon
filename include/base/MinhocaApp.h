#ifndef MINHOCAAPP_H
#define MINHOCAAPP_H

#include "MooseApp.h"

class MinhocaApp;

template<>
InputParameters validParams<MinhocaApp>();

class MinhocaApp : public MooseApp
{
public:
  MinhocaApp(const std::string & name, InputParameters parameters);
  virtual ~MinhocaApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* MINHOCAAPP_H */
