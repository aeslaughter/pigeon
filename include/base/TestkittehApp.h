#ifndef TESTKITTEHAPP_H
#define TESTKITTEHAPP_H

#include "MooseApp.h"

class TestkittehApp;

template<>
InputParameters validParams<TestkittehApp>();

class TestkittehApp : public MooseApp
{
public:
  TestkittehApp(const std::string & name, InputParameters parameters);
  virtual ~TestkittehApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* TESTKITTEHAPP_H */
