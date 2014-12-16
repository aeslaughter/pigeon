#ifndef LLAMAAPP_H
#define LLAMAAPP_H

#include "MooseApp.h"

class LlamaApp;

template<>
InputParameters validParams<LlamaApp>();

class LlamaApp : public MooseApp
{
public:
  LlamaApp(const std::string & name, InputParameters parameters);
  virtual ~LlamaApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* LLAMAAPP_H */
