#ifndef MUTT_1APP_H
#define MUTT_1APP_H

#include "MooseApp.h"

class Mutt1App;

template<>
InputParameters validParams<Mutt1App>();

class Mutt1App : public MooseApp
{
public:
  Mutt1App(const std::string & name, InputParameters parameters);
  virtual ~Mutt1App();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* MUTT_1APP_H */
