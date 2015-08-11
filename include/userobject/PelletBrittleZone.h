#ifndef PELLETBRITTLEZONE_H
#define PELLETBRITTLEZONE_H

#include "ElementUserObject.h"

class PelletBrittleZone;

template<>
InputParameters validParams<PelletBrittleZone>();

class PelletBrittleZone : public ElementUserObject
{

  public:
    PelletBrittleZone(const InputParameters & parameters);

    virtual void initialize();
    virtual void execute();
    virtual void threadJoin(const UserObject & u );
    virtual void finalize();

    Real brittleZoneWidth( Real r ) const;
    Real pelletRadius() const;
    Real pelletHeight() const;

  protected:
    std::map<unsigned int, unsigned int> _num_points;

    std::map<unsigned int, Real> _sum_x2;
    std::map<unsigned int, Real> _sum_x4;
    std::map<unsigned int, Real> _sum_y;
    std::map<unsigned int, Real> _sum_x2y;

    std::map<unsigned int, Real> _coeff_a;
    std::map<unsigned int, Real> _coeff_b;
    std::map<unsigned int, Real> _brittle_zone_width;

  private:
    VariableValue & _pellet_id;
    VariableValue & _temp;

    const Real _pellet_radius;
    const Real _a_lower;
    const Real _a_upper;
    const unsigned int _number_pellets;

    const Real _threshold_temperature;

};

#endif




