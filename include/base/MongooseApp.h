#ifndef MONGOOSEAPP_H
#define MONGOOSEAPP_H

#include "MooseApp.h"

class MongooseApp;

template<>
InputParameters validParams<MongooseApp>();

class MongooseApp : public MooseApp
{
public:
  MongooseApp(const std::string & name, InputParameters parameters);
  virtual ~MongooseApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* MONGOOSEAPP_H */
