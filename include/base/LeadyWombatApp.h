#ifndef LEADY_WOMBATAPP_H
#define LEADY_WOMBATAPP_H

#include "MooseApp.h"

class LeadyWombatApp;

template<>
InputParameters validParams<LeadyWombatApp>();

class LeadyWombatApp : public MooseApp
{
public:
  LeadyWombatApp(const std::string & name, InputParameters parameters);
  virtual ~LeadyWombatApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* LEADY_WOMBATAPP_H */
