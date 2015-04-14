#include "PelletBrittleZone.h"
#include <cmath>

template<>
InputParameters validParams<PelletBrittleZone>()
{
  InputParameters params = validParams<ElementUserObject>();

  params.addCoupledVar("pellet_id", "coupled pellet ID");
  params.addCoupledVar("temp", "coupled temperature (K)");
  params.addRequiredParam<Real>("pellet_radius", "initial pellet radius (m)");   // Preliminary (current pellet radius should be used)
  params.addRequiredParam<Real>("a_lower", "lower axial position (m)");          // Axial position at the bottom of the fuel stack
  params.addRequiredParam<Real>("a_upper", "initial upper axial position (m)");  // Preliminary (current fuel stack height should be considered)
  params.addRequiredParam<unsigned int>("number_pellets", "number of pellets");  // Number of fuel pellets

  MultiMooseEnum execute_options(SetupInterface::getExecuteOptions());
  execute_options = "residual";
  params.set<MultiMooseEnum>("execute_on") = execute_options;

  return params;
}

PelletBrittleZone :: PelletBrittleZone(const std::string & name, InputParameters parameters) :
  ElementUserObject(name, parameters),

  _pellet_id(coupledValue("pellet_id")),
  _temp(coupledValue("temp")),

  _pellet_radius(getParam<Real>("pellet_radius")),
  _a_lower(getParam<Real>("a_lower")),
  _a_upper(getParam<Real>("a_upper")),
  _number_pellets(getParam<unsigned int>("number_pellets")),

  _threshold_temperature(1200.+273.)  // Temperature below which fuel is regarded as completely brittle
{
}

void
PelletBrittleZone::initialize()
{
  for ( unsigned int i=1; i<=_number_pellets; ++i )
  {

    _num_points[i] = 0;

    _sum_x2[i]  = 0.;
    _sum_x4[i]  = 0.;
    _sum_y[i]   = 0.;
    _sum_x2y[i] = 0.;

    _coeff_a[i] = 0.;
    _coeff_b[i] = 0.;
    _brittle_zone_width[i] = 0.;
  }
}

void
PelletBrittleZone::execute()
{
  for ( unsigned int qp = 0; qp < _qrule->n_points(); ++qp )
  {

    if ( _temp[qp] < 0. )
    {
      if (n_processors()>1)
      {
        std::stringstream errorMsg;
        errorMsg << "PelletBrittleZone: Negative temperature:"
                 << "\nElement:     " << _current_elem->id()
                 << "\nqp:          " << qp
                 << "\nTemperature: " << _temp[qp];
        mooseError(errorMsg.str());
      }
      else
      {
        std::stringstream errorMsg;
        errorMsg << "\nPelletBrittleZone:: Negative temperature in element: " << _current_elem->id();
        Moose::out << errorMsg.str() << std::endl << std::endl;
        throw MooseException();
      }
    }

    unsigned int actual_pellet = _pellet_id[qp] + .5;  // conversion of pellet ID to integer
    Real radial_position = _q_point[qp](0);
    Real temperature = _temp[qp];

    _num_points[actual_pellet] += 1;

    _sum_x2[actual_pellet]  += pow(radial_position,2);
    _sum_x4[actual_pellet]  += pow(radial_position,4);
    _sum_y[actual_pellet]   += temperature;
    _sum_x2y[actual_pellet] += pow(radial_position,2) * temperature;

  }
}

void
PelletBrittleZone::threadJoin(const UserObject & u )
{
  const PelletBrittleZone & pbz = dynamic_cast<const PelletBrittleZone &>(u);

  for ( std::map<unsigned int, unsigned int>::const_iterator it = pbz._num_points.begin();
        it != _num_points.end();
        ++it )
    _num_points[it->first] += it->second;

  for ( std::map<unsigned int, Real>::const_iterator it = pbz._sum_x2.begin();
        it != _sum_x2.end();
        ++it )
    _sum_x2[it->first] += it->second;

  for ( std::map<unsigned int, Real>::const_iterator it = pbz._sum_x4.begin();
        it != _sum_x4.end();
        ++it )
    _sum_x4[it->first] += it->second;

  for ( std::map<unsigned int, Real>::const_iterator it = pbz._sum_y.begin();
        it != _sum_y.end();
        ++it )
    _sum_y[it->first] += it->second;

  for ( std::map<unsigned int, Real>::const_iterator it = pbz._sum_x2y.begin();
        it != _sum_x2y.end();
        ++it )
    _sum_x2y[it->first] += it->second;

  for ( std::map<unsigned int, Real>::const_iterator it = pbz._coeff_a.begin();
        it != _coeff_a.end();
        ++it )
    _coeff_a[it->first] += it->second;

  for ( std::map<unsigned int, Real>::const_iterator it = pbz._coeff_b.begin();
        it != _coeff_b.end();
        ++it )
    _coeff_b[it->first] += it->second;

  for ( std::map<unsigned int, Real>::const_iterator it = pbz._brittle_zone_width.begin();
        it != _brittle_zone_width.end();
        ++it )
    _brittle_zone_width[it->first] += it->second;
}

void
PelletBrittleZone::finalize()
{
  /* Draw a representative temperature profile of the form T = a^2 * r + b
     for each fuel pellet, using the least squares method.
     Then calculate the width of the brittle zone in the fuel pellet
     as the region cooler than the threshold temperature (D.R. Olander,
     Fundamental aspects of nuclear reactor fuel elements, Berkeley, 1976) */

  for(std::map<unsigned int, unsigned int>::iterator it = _num_points.begin();
      it != _num_points.end();
      ++it)
    gatherSum(it->second);

  for(std::map<unsigned int, Real>::iterator it = _sum_x2.begin();
      it != _sum_x2.end();
      ++it)
    gatherSum(it->second);

  for(std::map<unsigned int, Real>::iterator it = _sum_x4.begin();
      it != _sum_x4.end();
      ++it)
    gatherSum(it->second);

  for(std::map<unsigned int, Real>::iterator it = _sum_y.begin();
      it != _sum_y.end();
      ++it)
    gatherSum(it->second);

  for(std::map<unsigned int, Real>::iterator it = _sum_x2y.begin();
      it != _sum_x2y.end();
      ++it)
    gatherSum(it->second);

  for(std::map<unsigned int, Real>::iterator it = _coeff_a.begin();
      it != _coeff_a.end();
      ++it)
  {
    unsigned int id = it->first;

    Real factor = 1. / ( _num_points[id]*_sum_x4[id] - pow(_sum_x2[id],2) );
    _coeff_a[id] = factor * ( _num_points[id]*_sum_x2y[id] - _sum_x2[id]*_sum_y[id] );

  }

  for(std::map<unsigned int, Real>::iterator it = _coeff_b.begin();
      it != _coeff_b.end();
      ++it)
  {
    unsigned int id = it->first;

    Real factor = 1. / ( _num_points[id]*_sum_x4[id] - pow(_sum_x2[id],2) );
    _coeff_b[id] = factor * ( - _sum_x2[id]*_sum_x2y[id] + _sum_x4[id]*_sum_y[id] );

    if ( _coeff_b[id] < 0. )
      mooseError("From PelletBrittleZone: error occured during calculation of pellet temperature profile");
  }

  for(std::map<unsigned int, Real>::iterator it = _brittle_zone_width.begin();
      it != _brittle_zone_width.end();
      ++it)
  {
    unsigned int id = it->first;

    if ( _coeff_b[id] < _threshold_temperature )
      _brittle_zone_width[id] = _pellet_radius;

    else if ( _coeff_a[id] >= 0. )
      _brittle_zone_width[id] = 0.;

    else
      _brittle_zone_width[id] = _pellet_radius - sqrt( (_threshold_temperature-_coeff_b[id])/_coeff_a[id] );
  }
}

Real
PelletBrittleZone::brittleZoneWidth( Real r ) const
{
  unsigned int i = r + .5;  // conversion of pellet ID to integer

  if ( _brittle_zone_width.find(i) != _brittle_zone_width.end() )
    return _brittle_zone_width.find(i)->second;

  mooseError("Error occurred in method brittleZoneWidth of PelletBrittleZone (unknown pellet ID requested)");
    return 0;
}

Real
PelletBrittleZone::pelletRadius() const
{
  return _pellet_radius;
}

Real
PelletBrittleZone::pelletHeight() const
{
  const Real  _pellet_height = (_a_upper-_a_lower)/_number_pellets;
  return _pellet_height;
}
