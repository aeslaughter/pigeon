#ifndef Sifgrs_H
#define Sifgrs_H

#include "Material.h"
#include "Function.h"
#include "PelletBrittleZone.h"

//Forward Declarations
class BurnupFunction;

class Sifgrs : public Material
{

public:
  Sifgrs(const InputParameters & parameters);

protected:
  virtual void initStatefulProperties(unsigned n_points);
  virtual void computeProperties();

private:
  static bool _write_msg;

  bool parseNextLineReals(std::ifstream & ifs, std::vector<Real> & myvec);
  void parseRows( std::vector<Real> & , std::vector<Real> & y );
  void parseColumns( std::vector<Real> & x, std::vector<Real> & y);

  Real getFissionRate(unsigned qp);
  Real getFissionRateOld(unsigned qp);
  Real getBurnup(unsigned qp);
  Real getBurnupOld(unsigned qp);

  const Real _initial_grain_radius;
  const Real _hydrostatic_stress_const;
  const Real _initial_sat_coverage;
  const Real _hbs_release_burnup;

  const Real _initial_porosity;
  const Real _initial_density;

  const Real _solid_swelling_factor;
  const Real _total_densification;
  const Real _end_densification_burnup;

  const Real _temperature_scalef;
  const Real _grainradius_scalef;
  const Real _igdiffcoeff_scalef;
  const Real _resolutionp_scalef;
  const Real _gbdiffcoeff_scalef;

  const PelletBrittleZone * const _pellet_brittle_zone;

  const short int _diff_coeff_option;
  const short int _res_param_option;
  const short int _transient_option;

  bool _has_grain_radius;
  VariableValue & _grain_radius;
  VariableValue & _grain_radius_old;

  bool _has_pellet_id;
  VariableValue & _pellet_id;

  bool _has_fission_rate;
  VariableValue & _fission_rate;
  VariableValue & _fission_rate_old;

  bool _has_temp;
  VariableValue & _temp;
  VariableValue & _temp_old;

  bool _has_hydrostatic_stress;
  VariableValue & _hydrostatic_stress;
  VariableValue & _hydrostatic_stress_old;

  bool _has_burnup;
  VariableValue & _burnup;
  VariableValue & _burnup_old;

  BurnupFunction * const _burnup_function;

  const Real avogadros_num;
  const Real boltzmann_const;
  const Real vanderwaals_b;

  const Real act_energy;
  const Real vcn_volume;
  const Real grb_thickness;

  const Real fract_yield;
  const Real fissp_length;
  const Real fissp_radius;
  const Real horder_knockon_range;

  const Real rad_bbl_grn_i;

  const Real buthr_hbs;
  const Real buconst;

  const Real surface_tension;

  const Real sd_angle;
  const Real geom_fact;

  const Real bbl_bdr_2_i;
  const Real rad_bbl_bdr_i;

  const Real temp_center;
  const Real temp_span;

  MaterialProperty<Real> & _actual_density;

  MaterialProperty<Real> & _linear_power;
  MaterialProperty<Real> & _linear_power_old;

  MaterialProperty<Real> & _burnup_loc;
  MaterialProperty<Real> & _burnup_loc_old;

  MaterialProperty<Real> & _gas_gen_3;
  MaterialProperty<Real> & _gas_grn_3;
  MaterialProperty<Real> & _gas_gbs_3;
  MaterialProperty<Real> & _gas_ath_3;
  MaterialProperty<Real> & _gas_rpr_3;
  MaterialProperty<Real> & _gas_hbs_3;
  MaterialProperty<Real> & _gas_hbr_3;
  MaterialProperty<Real> & _gas_bdr_2;
  MaterialProperty<Real> & _gas_bdr_3;
  MaterialProperty<Real> & _gas_thr_2;
  MaterialProperty<Real> & _gas_thr_3;
  MaterialProperty<Real> & _gas_trn_2;
  MaterialProperty<Real> & _gas_trn_3;
  MaterialProperty<Real> & _gas_rel_3;

  MaterialProperty<Real> & _gas_gen_3_old;
  MaterialProperty<Real> & _gas_grn_3_old;
  MaterialProperty<Real> & _gas_gbs_3_old;
  MaterialProperty<Real> & _gas_ath_3_old;
  MaterialProperty<Real> & _gas_rpr_3_old;
  MaterialProperty<Real> & _gas_hbs_3_old;
  MaterialProperty<Real> & _gas_hbr_3_old;
  MaterialProperty<Real> & _gas_bdr_3_old;
  MaterialProperty<Real> & _gas_thr_3_old;
  MaterialProperty<Real> & _gas_trn_3_old;

  MaterialProperty<Real> & _crack_length;
  MaterialProperty<Real> & _crack_number;

  MaterialProperty<Real> & _g_0;
  MaterialProperty<Real> & _g_1;
  MaterialProperty<Real> & _g_2;
  MaterialProperty<Real> & _g_3;

  MaterialProperty<Real> & _g_0_old;
  MaterialProperty<Real> & _g_1_old;
  MaterialProperty<Real> & _g_2_old;
  MaterialProperty<Real> & _g_3_old;

  MaterialProperty<Real> & _bbl_grn_3;
  MaterialProperty<Real> & _rad_bbl_grn;
  MaterialProperty<Real> & _satom_fract_grn;

  MaterialProperty<Real> & _eff_diff_coeff;
  MaterialProperty<Real> & _eff_diff_coeff_old;

  MaterialProperty<bool> & _rampup_cond;

  MaterialProperty<Real> & _bbl_bdr_2;
  MaterialProperty<Real> & _bbl_bdr_3;
  MaterialProperty<Real> & _vcn_bdr_2;
  MaterialProperty<Real> & _atm_bbl_bdr;
  MaterialProperty<Real> & _vcn_bbl_bdr;
  MaterialProperty<Real> & _rad_bbl_bdr;
  MaterialProperty<Real> & _arp_bbl_bdr;
  MaterialProperty<Real> & _vol_bbl_bdr;
  MaterialProperty<Real> & _GBCoverage;
  MaterialProperty<Real> & _sat_coverage;

  MaterialProperty<Real> & _bbl_bdr_3_old;
  MaterialProperty<Real> & _vcn_bdr_2_old;
  MaterialProperty<Real> & _rad_bbl_bdr_old;
  MaterialProperty<Real> & _arp_bbl_bdr_old;
  MaterialProperty<Real> & _GBCoverage_old;
  MaterialProperty<Real> & _sat_coverage_old;

  MaterialProperty<Real> & _prs_bbl_bdr;
  MaterialProperty<Real> & _prseq_bbl_bdr;

  MaterialProperty<Real> & _fract_active_faces;

  MaterialProperty<Real> & _fract_active_faces_old;

  MaterialProperty<Real> & _deltav_v0_bd;
  MaterialProperty<Real> & _deltav_v0_sl;
  MaterialProperty<Real> & _deltav_v0_dn;
  MaterialProperty<Real> & _deltav_v0_swe;

  MaterialProperty<Real> & _deltav_v0_bd_old;
  MaterialProperty<Real> & _deltav_v0_sl_old;
  MaterialProperty<Real> & _deltav_v0_dn_old;
  MaterialProperty<Real> & _deltav_v0_swe_old;

  MaterialProperty<Real> & _fabrication_porosity;
  MaterialProperty<Real> & _gaseous_porosity;
  MaterialProperty<Real> & _sintering_porosity;

  const bool _skip_bdr_model;
  const bool _compute_swelling;
  const bool _testing_output;

  const bool _ath_model;
  const bool _gbs_model;
  const bool _ramp_model;
  const bool _hbs_model;

  const std::string _file_name;
  const std::string _format;

  Function * const _rod_ave_lin_pow;
  Function * const _axial_power_profile;

  MaterialProperty<Real>* _fract_FGR_analytical;

  Real grn_radius;
  Real grn_radius_old;

  std::vector<Real> a_coeffs;
  std::vector<Real> b_coeffs;

  Real ddeltav_v0_bd;
  Real dddeltav_v0_bd_dT;
  Real ddeltav_v0_sl;
  Real dddeltav_v0_sl_dT;
  Real ddeltav_v0_dn;
  Real dddeltav_v0_dn_dT;
  Real ddeltav_v0_swe;
  Real dddeltav_v0_swe_dT;

  std::vector<Real> time_lp;
  std::vector<Real> lp_input;

};

template<>
InputParameters validParams<Sifgrs>();

#endif // Sifgrs_H
