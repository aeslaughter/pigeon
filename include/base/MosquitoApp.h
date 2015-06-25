#ifndef MOSQUITOAPP_H
#define MOSQUITOAPP_H

#include "MooseApp.h"

class MosquitoApp;

template<>
InputParameters validParams<MosquitoApp>();

class MosquitoApp : public MooseApp
{
public:
  MosquitoApp(const std::string & name, InputParameters parameters);
  virtual ~MosquitoApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* MOSQUITOAPP_H */
