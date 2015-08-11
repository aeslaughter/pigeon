#include "Sifgrs.h"

#include "BurnupFunction.h"
#include "VSwellingUO2.h"
#include "SymmTensor.h"

#include "MooseException.h"

#include <cmath>

/* This routine computes fission gas release and fuel swelling according to a
   Simple Integrated Fission Gas Release and Swelling (SIFGRS) model */

template<>
InputParameters validParams<Sifgrs>()
{
  InputParameters params = validParams<Material>();

  params.addParam<Real>("initial_grain_radius", 5.e-06, "initial grain radius (m)"); // Pre-irradiation fuel grain radius [m]
  params.addParam<Real>("hydrostatic_stress_const", 0., "constant hydrostatic stress (Pa)"); // Allow user to adopt a constant value for hydrostatic stress [Pa]
  params.addParam<Real>("saturation_coverage", 0.5, "saturation coverage (/)");   // Initial value of the fractional grain boundary gas bubble coverage at saturation [/]
  params.addParam<Real>("hbs_release_burnup", 100., "HBS gas release threshold (MWd/kgU)");  // Threshold local burnup for gas release from the HBS porosity [MWd/kgU]

  params.addParam<Real>("initial_porosity", 0.05, "initial fuel porosity (/)");
  params.addRequiredParam<Real>("density", "initial fuel density (kg/m**3)");

  params.addPrivateParam<Real>("solid_swelling_factor", 5.577e-5);
  params.addParam<Real>("total_densification", 0.01, "final densification given as a fraction of theoretical density");
  params.addParam<Real>("end_densification_burnup", 5., "burnup at which densification is complete (MWd/kgU)");

  params.addParam<Real>("temperature_scalef", 1., "scaling factor for temperature");
  params.addParam<Real>("grainradius_scalef", 1., "scaling factor for grain radius");
  params.addParam<Real>("igdiffcoeff_scalef", 1., "scaling factor for intragranular diffusion coefficient");
  params.addParam<Real>("resolutionp_scalef", 1., "scaling factor for resolution parameter");
  params.addParam<Real>("gbdiffcoeff_scalef", 1., "scaling factor for grain boundary diffusion coefficient");

  params.addParam<UserObjectName>("pellet_brittle_zone", "The name of the UserObject that computes the width of the brittle zone in the fuel pellet");

  params.addParam<short int>("diff_coeff_option", 3, "Select diffusion coefficient");
  params.addParam<short int>("res_param_option", 0, "Select resolution coefficient");

  params.addParam<short int>("transient_option", 0, "Select transient release model");

  params.addPrivateParam<bool>("compute_swelling", false);
  params.addPrivateParam<bool>("skip_bdr_model", false);
  params.addParam<std::string>("swelling_name", "deltav_v0_swe", "The name to give the scalar swelling material property");
  params.addPrivateParam<bool>("testing_output", false);

  params.addPrivateParam<bool>("ath_model", false);
  params.addPrivateParam<bool>("gbs_model", false);
  params.addPrivateParam<bool>("ramp_model", false);
  params.addPrivateParam<bool>("hbs_model", false);

  params.addParam<FileName>("file_name", "csv file holding linear power vs. time");
  params.addParam<std::string>("format", "rows" ,"format of csv data file that is either in columns or rows");
  params.addParam<FunctionName>("rod_ave_lin_pow", "", "linear power function.");
  params.addParam<FunctionName>("axial_power_profile", "", "axial power peaking function.");

  params.addCoupledVar("grain_radius", "Coupled Grain Radius");
  params.addCoupledVar("pellet_id", "Coupled Pellet ID");
  params.addCoupledVar("temp", "Coupled Temperature");
  params.addCoupledVar("fission_rate", "Coupled Fission Rate");
  params.addCoupledVar("hydrostatic_stress", "Coupled Hydrostatic Stress");
  params.addCoupledVar("burnup", "Coupled Burnup");
  params.addParam<FunctionName>("burnup_function", "Burnup function");

  return params;
}

bool Sifgrs::_write_msg = true;

Sifgrs::Sifgrs(const InputParameters & parameters) :
  Material(parameters),
  _initial_grain_radius(getParam<Real>("initial_grain_radius")),
  _hydrostatic_stress_const(getParam<Real>("hydrostatic_stress_const")),
  _initial_sat_coverage(getParam<Real>("saturation_coverage")),
  _hbs_release_burnup(getParam<Real>("hbs_release_burnup")),
  _initial_porosity(getParam<Real>("initial_porosity")),
  _initial_density(getParam<Real>("density")),

  _solid_swelling_factor(getParam<Real>("solid_swelling_factor")),
  _total_densification(getParam<Real>("total_densification")),
  _end_densification_burnup(getParam<Real>("end_densification_burnup")),

  /* Scaling factors for sensitivity analysis */
  _temperature_scalef(getParam<Real>("temperature_scalef")),
  _grainradius_scalef(getParam<Real>("grainradius_scalef")),
  _igdiffcoeff_scalef(getParam<Real>("igdiffcoeff_scalef")),
  _resolutionp_scalef(getParam<Real>("resolutionp_scalef")),
  _gbdiffcoeff_scalef(getParam<Real>("gbdiffcoeff_scalef")),

  _pellet_brittle_zone(isParamValid("pellet_brittle_zone") ? &getUserObject<PelletBrittleZone>("pellet_brittle_zone") : NULL),

  _diff_coeff_option(getParam<short int>("diff_coeff_option")),
  _res_param_option(getParam<short int>("res_param_option")),

  _transient_option(getParam<short int>("transient_option")),

  /* Coupled variables */

  _has_grain_radius(isCoupled("grain_radius")),
  _grain_radius(_has_grain_radius?coupledValue("grain_radius"):_zero),  // grain radius [m]
  _grain_radius_old(_has_grain_radius?coupledValueOld("grain_radius"):_zero),
  _has_pellet_id(isCoupled("pellet_id")),
  _pellet_id(_has_pellet_id?coupledValue("pellet_id"):_zero),  // pellet ID
  _has_fission_rate(isCoupled("fission_rate")),
  _fission_rate(_has_fission_rate?coupledValue("fission_rate"):_zero),  // fission rate [1/(m**3*s)]
  _fission_rate_old(_has_fission_rate?coupledValueOld("fission_rate"):_zero),
  _has_temp(isCoupled("temp")),
  _temp(_has_temp?coupledValue("temp"):_zero),  // temperature [K]
  _temp_old(_has_temp?coupledValueOld("temp"):_zero),
  _has_hydrostatic_stress(isCoupled("hydrostatic_stress")),
  _hydrostatic_stress(_has_hydrostatic_stress?coupledValue("hydrostatic_stress"):_zero), // local hydrostatic stress in the fuel [Pa]
  _hydrostatic_stress_old(_has_hydrostatic_stress?coupledValueOld("hydrostatic_stress"):_zero),
  _has_burnup(isCoupled("burnup")),
  _burnup(_has_burnup?coupledValue("burnup"):_zero),  // local burnup [FIMA]
  _burnup_old(_has_burnup?coupledValueOld("burnup"):_zero),

   _burnup_function( isParamValid("burnup_function") ?
                     dynamic_cast<BurnupFunction*>(&getFunction("burnup_function")) : NULL ),

  /* Parameters */

  // Physical constants

  avogadros_num(6.022e+23),       // Avogadro's number [at/mol]
  boltzmann_const(1.380651e-23),  // Boltzmann constant [J/K]
  vanderwaals_b(5.105e-05),       // Van der Waals constant b of Xe [m**3/mol]

  act_energy(5.75e-19),    // activation energy for grain-boundary diffusion of vacancies [J]
  vcn_volume(4.09e-29),    // atomic (vacancy) volume in grain-boundary bubbles [m**3]
  grb_thickness(3.5e-10),  // thickness of the diffusion layer on grain boundaries [m]

  // Intragranular and athermal release model parameters

  fract_yield(0.3017),   // fractional yield of fission gas atoms (Xe + Kr) [/]
  fissp_length(6.e-06),  // length of a fission spike [m]
  fissp_radius(1.e-09),  // radius of influence of a fission spike [m]
  horder_knockon_range(5.e-09),  // range of the higher-order uranium knock-on in UO2 [m]

  rad_bbl_grn_i(5.e-10),  // initial intragranular bubble radius of curvature [m]

  // HBS model parameters

  buthr_hbs(70.),   // threshold local burnup for the formation of the HBS [MWd/kgU]
  buconst(7.e-02),  // fitting constant [1/(MWd/kgU)]

  // Grain-boundary model parameters

  surface_tension (0.5),     // UO2-gas surface tension [J/m**2]

  sd_angle(0.872665),  // semidihedral angle of grain-boundary bubbles [rad]
  geom_fact(0.168611), // geometric factor relating the volume of a lenticular-shape bubble to that of a sphere,
                       // geom_fact = 1. - 1.5 * cos(sd_angle) + 0.5 * (cos(sd_angle))**3

  bbl_bdr_2_i(2.e13),     // initial number density of grain-boundary bubbles [bub/m**2]
  rad_bbl_bdr_i(1.e-08),  // initial grain-boundary bubble radius of curvature [m]

  // Transient gas release

  temp_center(1773.), // sigmoid central parameter [K]
  temp_span(100.),    // sigmoid width parameter [K]

  /* Model variables */

  // Get actual fuel density

  _actual_density(getMaterialProperty<Real>("density")),

  // Local linear heat rate

  _linear_power(declareProperty<Real>("linear_power")),  // rod linear power at current axial position [W/m]
  _linear_power_old(declarePropertyOld<Real>("linear_power")),

  // Local burnup

  _burnup_loc(declareProperty<Real>("burnup_loc")),  // local burnup [MWd/kgU]
  _burnup_loc_old(declarePropertyOld<Real>("burnup_loc")),

  // Gas concentrations

  _gas_gen_3(declareProperty<Real>("gas_gen_3")),  // volume concentration of gas produced [mol/m**3]
  _gas_grn_3(declareProperty<Real>("gas_grn_3")),  // volume concentration of gas in grains [mol/m**3]
  _gas_gbs_3(declareProperty<Real>("gas_gbs_3")),  // volume concentration of gas swept by growing grains [mol/m**3]
  _gas_ath_3(declareProperty<Real>("gas_ath_3")),  // volume concentration of gas released due to athermal release [mol/m**3]
  _gas_rpr_3(declareProperty<Real>("gas_rpr_3")),  // volume concentration of gas released due to ramp release [mol/m**3]
  _gas_hbs_3(declareProperty<Real>("gas_hbs_3")),  // volume concentration of gas in HBS porosity [mol/m**3]
  _gas_hbr_3(declareProperty<Real>("gas_hbr_3")),  // volume concentration of gas released from HBS porosity [mol/m**3]
  _gas_bdr_2(declareProperty<Real>("gas_bdr_2")),  // surface concentration of gas in grain boundaries [mol/m**2]
  _gas_bdr_3(declareProperty<Real>("gas_bdr_3")),  // volume concentration of gas in grain boundaries [mol/m**3]
  _gas_thr_2(declareProperty<Real>("gas_thr_2")),  // surface concentration of gas released due to thermal release [mol/m**2]
  _gas_thr_3(declareProperty<Real>("gas_thr_3")),  // volume concentration of gas released due to thermal release [mol/m**3]
  _gas_trn_2(declareProperty<Real>("gas_trn_2")),  // surface concentration of gas released due to transient release [mol/m**2]
  _gas_trn_3(declareProperty<Real>("gas_trn_3")),  // volume concentration of gas released due to transient release [mol/m**3]
  _gas_rel_3(declareProperty<Real>("gas_rel_3")),  // volume concentration of gas released (total) [mol/m**3]

  _gas_gen_3_old(declarePropertyOld<Real>("gas_gen_3")),
  _gas_grn_3_old(declarePropertyOld<Real>("gas_grn_3")),
  _gas_gbs_3_old(declarePropertyOld<Real>("gas_gbs_3")),
  _gas_ath_3_old(declarePropertyOld<Real>("gas_ath_3")),
  _gas_rpr_3_old(declarePropertyOld<Real>("gas_rpr_3")),
  _gas_hbs_3_old(declarePropertyOld<Real>("gas_hbs_3")),
  _gas_hbr_3_old(declarePropertyOld<Real>("gas_hbr_3")),
  _gas_bdr_3_old(declarePropertyOld<Real>("gas_bdr_3")),
  _gas_thr_3_old(declarePropertyOld<Real>("gas_thr_3")),
  _gas_trn_3_old(declarePropertyOld<Real>("gas_trn_3")),

  // Athermal release model

  _crack_length(declareProperty<Real>("crack_length")),
  _crack_number(declareProperty<Real>("crack_number")),

  // Intragranular gas diffusion model

  _g_0(declareProperty<Real>("g_0")),
  _g_1(declareProperty<Real>("g_1")),
  _g_2(declareProperty<Real>("g_2")),
  _g_3(declareProperty<Real>("g_3")),

  _g_0_old(declarePropertyOld<Real>("g_0")),
  _g_1_old(declarePropertyOld<Real>("g_1")),
  _g_2_old(declarePropertyOld<Real>("g_2")),
  _g_3_old(declarePropertyOld<Real>("g_3")),

  _bbl_grn_3(declareProperty<Real>("bbl_grn_3")),             // number density of intragranular bubbles [bub/m**3]
  _rad_bbl_grn(declareProperty<Real>("rad_bbl_grn")),         // intragranular bubble radius [m]
  _satom_fract_grn(declareProperty<Real>("satom_fract_grn")), // fraction of intragranular gas existing as single atoms [/]

  _eff_diff_coeff(declareProperty<Real>("eff_diff_coeff")),  // effective diffusion coefficient [m**2/s]
  _eff_diff_coeff_old(declarePropertyOld<Real>("eff_diff_coeff")),

  // Ramp release model

  _rampup_cond(declareProperty<bool>("rampup_cond")),

  // Grain-boundary model

  _bbl_bdr_2(declareProperty<Real>("bbl_bdr_2")),      // number density of grain-boundary bubbles [bub/m**2]
  _bbl_bdr_3(declareProperty<Real>("bbl_bdr_3")),      // number density of grain-boundary bubbles [bub/m**3]
  _vcn_bdr_2(declareProperty<Real>("vcn_bdr_2")),      // surface concentration of vacancies at grain boundaries [vac/m**2]
  _atm_bbl_bdr(declareProperty<Real>("atm_bbl_bdr")),  // number of gas atoms per grain-boundary bubble [at/bub]
  _vcn_bbl_bdr(declareProperty<Real>("vcn_bbl_bdr")),  // number of vacancies per grain-boundary bubble [vac/bub]
  _rad_bbl_bdr(declareProperty<Real>("rad_bbl_bdr")),  // grain-boundary bubble radius of curvature [m]
  _arp_bbl_bdr(declareProperty<Real>("arp_bbl_bdr")),  // grain-boundary bubble projected area [m**2]
  _vol_bbl_bdr(declareProperty<Real>("vol_bbl_bdr")),  // grain-boundary bubble volume [m**3]
  _GBCoverage(declareProperty<Real>("GBCoverage")),    // fractional grain boundary bubble coverage [/]
  _sat_coverage(declareProperty<Real>("sat_coverage")),// grain boundary bubble coverage at saturation [/]

  _bbl_bdr_3_old(declarePropertyOld<Real>("bbl_bdr_3")),
  _vcn_bdr_2_old(declarePropertyOld<Real>("vcn_bdr_2")),
  _rad_bbl_bdr_old(declarePropertyOld<Real>("rad_bbl_bdr")),
  _arp_bbl_bdr_old(declarePropertyOld<Real>("arp_bbl_bdr")),
  _GBCoverage_old(declarePropertyOld<Real>("GBCoverage")),
  _sat_coverage_old(declarePropertyOld<Real>("sat_coverage")),

  _prs_bbl_bdr(declareProperty<Real>("prs_bbl_bdr")),      // internal pressure of grain-bounday gas bubbles [Pa]
  _prseq_bbl_bdr(declareProperty<Real>("prseq_bbl_bdr")),  // internal pressure of grain-bounday gas bubbles at mechanical equilibrium [Pa]

  // Transient release model

  _fract_active_faces(declareProperty<Real>("fract_active_faces")), // fraction of active grain faces [/]

  _fract_active_faces_old(declarePropertyOld<Real>("fract_active_faces")),

  // Swelling model

  _deltav_v0_bd(declareProperty<Real>("deltav_v0_bd")),  // Fractional volume grain-boundary fission gas swelling [/]
  _deltav_v0_sl(declareProperty<Real>("deltav_v0_sl")),  // Fractional volume solid swelling [/]
  _deltav_v0_dn(declareProperty<Real>("deltav_v0_dn")),  // Fractional volume densification [/]
  _deltav_v0_swe(declareProperty<Real>(getParam<std::string>("swelling_name"))), // Fractional volume swelling (gas + solid + densification) [/]

  _deltav_v0_bd_old(declarePropertyOld<Real>("deltav_v0_bd")),
  _deltav_v0_sl_old(declarePropertyOld<Real>("deltav_v0_sl")),
  _deltav_v0_dn_old(declarePropertyOld<Real>("deltav_v0_dn")),
  _deltav_v0_swe_old(declarePropertyOld<Real>(getParam<std::string>("swelling_name"))),

  // Contributions to fuel porosity

  _fabrication_porosity(declareProperty<Real>("fabrication_porosity")), // Contribution due to fabrication pores [/]
  _gaseous_porosity(declareProperty<Real>("gaseous_porosity")),         // Contribution due to fission gas bubbles [/]
  _sintering_porosity(declareProperty<Real>("sintering_porosity")),     // Contribution due to densification [/]

  // Other variables

  _skip_bdr_model(getParam<bool>("skip_bdr_model")),
  _compute_swelling(getParam<bool>("compute_swelling")),
  _testing_output(getParam<bool>("testing_output")),

  _ath_model(getParam<bool>("ath_model")),
  _gbs_model(getParam<bool>("gbs_model")),
  _ramp_model(getParam<bool>("ramp_model")),
  _hbs_model(getParam<bool>("hbs_model")),

  _file_name(isParamValid("file_name") ? getParam<FileName>("file_name") : ""),
  _format(getParam<std::string>("format") ),
  _rod_ave_lin_pow(getParam<FunctionName>("rod_ave_lin_pow") != "" ? &getFunction("rod_ave_lin_pow") : NULL ),
  _axial_power_profile(getParam<FunctionName>("axial_power_profile") != "" ? &getFunction("axial_power_profile") : NULL ),

  _fract_FGR_analytical(_testing_output ? &declareProperty<Real>("fract_FGR_analytical") : NULL)

  {
    // Fuel grain radius

    grn_radius     = _initial_grain_radius;
    grn_radius_old = _initial_grain_radius;

    // Coefficients of the intragranular diffusion algorithm, used to approximate the integration kernel.
    // 4 term approximation with values from Hermansson and Massih (JNM 304, 204, 2002).

    a_coeffs.resize(4);
    a_coeffs[0] = 0.231545;
    a_coeffs[1] = 0.0948453;
    a_coeffs[2] = 0.0282898;
    a_coeffs[3] = 0.645320;

    b_coeffs.resize(4);
    b_coeffs[0] = 72.5968;
    b_coeffs[1] = 796.773;
    b_coeffs[2] = 29083.0;
    b_coeffs[3] = 10.2469;

    // Swelling increments

    ddeltav_v0_bd = 0.;
    dddeltav_v0_bd_dT = 0.;

    ddeltav_v0_sl = 0.;
    dddeltav_v0_sl_dT = 0.;

    ddeltav_v0_dn = 0.;
    dddeltav_v0_dn_dT = 0.;

    ddeltav_v0_swe = 0.;
    dddeltav_v0_swe_dT = 0.;

    // Checks

    if ( _diff_coeff_option < 0 || _diff_coeff_option > 3 )
    {
      std::stringstream diffCoeffErrorMsg;
      diffCoeffErrorMsg << "From Sifgrs: diff_coeff_option = " << _diff_coeff_option << " is not valid."
                        << " Valid options are integers in range [0,3]";
      mooseError(diffCoeffErrorMsg.str());
    }

    if ( _res_param_option < 0 || _res_param_option > 1 )
    {
      std::stringstream resParamErrorMsg;
      resParamErrorMsg << "From Sifgrs: res_param_option = " << _res_param_option << " is not valid."
                        << " Valid options are 0 and 1";
      mooseError(resParamErrorMsg.str());
    }

    if ( _transient_option < 0 || _transient_option > 2 )
    {
      std::stringstream tranErrorMsg;
      tranErrorMsg << "From Sifgrs: transient_option = " << _transient_option << " is not valid."
                        << " Valid options are 0, 1 and 2";
      mooseError(tranErrorMsg.str());
    }

    if ( !(_has_fission_rate || _burnup_function) )
      mooseError("From Sifgrs: fission_rate needed");

    if ( !_has_temp )
      mooseError("From Sifgrs: temp needed");

    if ( _compute_swelling )
    {
      if ( !_has_burnup )
        mooseError("burnup needed by Sifgrs if compute_swelling = true");
    }

    if ( _ath_model )
    {
      if ( !_rod_ave_lin_pow )
        mooseError("Linear power function rod_ave_lin_pow needed by Sifgrs if ath_model = true");

      if ( !_has_pellet_id )
        mooseError("pellet_id needed by Sifgrs if ath_model = true");

      if ( !_pellet_brittle_zone )
        mooseError("User object pellet_brittle_zone of type PelletBrittleZone needed by Sifgrs if ath_model = true");
    }

    if ( ( _hbs_model || _ramp_model) && !_has_burnup )
      mooseError("Burnup needed by Sifgrs if hbs_model = true or ramp_model = true");

    if ( _gbs_model && !_has_grain_radius )
      mooseError("grain_radius needed by Sifgrs if gbs_model = true");

    if ( _hbs_release_burnup < buthr_hbs )
    {
      std::stringstream errorMsg;
      errorMsg << "From Sifgrs: hbs_release_burnup must be >= " << buthr_hbs;
      mooseError(errorMsg.str());
    }

    if ( _ramp_model )
    {
      // Read input linear power vs. time
      if ( _format.compare(0, 4, "rows") == 0 )
        parseRows( time_lp, lp_input );
      else if ( _format.compare(0, 7, "columns") == 0 )
        parseColumns( time_lp, lp_input );
      else
        mooseError("Invalid option for format: "+_format+" in Sifgrs.  Valid options are rows and columns.");

      if ( ! _rod_ave_lin_pow )
        mooseError("Linear power function rod_ave_lin_pow needed by Sifgrs if ramp_model = true");
    }

    if ( _skip_bdr_model && _compute_swelling )
    {
      std::stringstream skipBdrerrorMsg;
      skipBdrerrorMsg << "From Sifgrs: skip_bdr_model = true does not allow compute_swelling = true";
      mooseError(skipBdrerrorMsg.str());
    }

    // flags
    if (_write_msg)
    {
      Moose::out << "Diffusion coefficient option #" << _diff_coeff_option << " selected for Sifgrs\n";
      if ( _ath_model )  Moose::out << "Athermal gas release capability in Sifgrs is activated\n";
      if ( _gbs_model )  Moose::out << "Grain boundary sweeping capability in Sifgrs is activated\n";
      if ( _ramp_model)  Moose::out << "Fission gas ramp release capability in Sifgrs is activated\n";
      if ( _hbs_model )  Moose::out << "High burnup structure gas release capability in Sifgrs is activated\n";
      if ( _transient_option == 1 || _transient_option == 2 )  Moose::out << "Transient gas release capability (model #" << _transient_option << ") in Sifgrs is activated\n";
      _write_msg = false;
    }
  }

void
Sifgrs::initStatefulProperties(unsigned n_points)
{
  for (unsigned qp(0); qp < n_points; ++qp)
  {
    _actual_density[qp] = _initial_density;

    _linear_power[qp] = 0.;
    _linear_power_old[qp] = 0.;

    _burnup_loc[qp] = 0.;
    _burnup_loc_old[qp] = 0.;

    _gas_gen_3[qp] = 0.;
    _gas_grn_3[qp] = 0.;
    _gas_gbs_3[qp] = 0.;
    _gas_ath_3[qp] = 0.;
    _gas_rpr_3[qp] = 0.;
    _gas_hbs_3[qp] = 0.;
    _gas_hbr_3[qp] = 0.;
    _gas_bdr_2[qp] = 0.;
    _gas_bdr_3[qp] = 0.;
    _gas_thr_2[qp] = 0.;
    _gas_thr_3[qp] = 0.;
    _gas_trn_2[qp] = 0.;
    _gas_trn_3[qp] = 0.;
    _gas_rel_3[qp] = 0.;

    _gas_gen_3_old[qp] = 0.;
    _gas_grn_3_old[qp] = 0.;
    _gas_gbs_3_old[qp] = 0.;
    _gas_rpr_3_old[qp] = 0.;
    _gas_hbs_3_old[qp] = 0.;
    _gas_hbr_3_old[qp] = 0.;
    _gas_bdr_3_old[qp] = 0.;
    _gas_trn_3_old[qp] = 0.;
    _gas_thr_3_old[qp] = 0.;

    _crack_length[qp] = 0.;
    _crack_number[qp] = 0.;

    _g_0[qp] = 0.;
    _g_1[qp] = 0.;
    _g_2[qp] = 0.;
    _g_3[qp] = 0.;

    _g_0_old[qp] = 0.;
    _g_1_old[qp] = 0.;
    _g_2_old[qp] = 0.;
    _g_3_old[qp] = 0.;

    _bbl_grn_3[qp] = 0.;
    _rad_bbl_grn[qp] = rad_bbl_grn_i;
    _satom_fract_grn[qp] = 1.;

    _eff_diff_coeff[qp] = 1.e-35;
    _eff_diff_coeff_old[qp] = 1.e-35;

    _rampup_cond[qp] = false;

    _vcn_bdr_2[qp] = 0.;
    _atm_bbl_bdr[qp] = 0.;
    _vcn_bbl_bdr[qp] = 0.;

    if ( ! _skip_bdr_model )
    {
      _bbl_bdr_2[qp] = bbl_bdr_2_i;
      _bbl_bdr_3[qp] = bbl_bdr_2_i * 3./_initial_grain_radius*(1.-_initial_porosity) / 2.;
      _rad_bbl_bdr[qp] = rad_bbl_bdr_i;
      _arp_bbl_bdr[qp] = M_PI * pow( sin(sd_angle), 2 ) * pow( rad_bbl_bdr_i, 2 );
      _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow(rad_bbl_bdr_i,3);
      _GBCoverage[qp] = bbl_bdr_2_i * M_PI * pow( sin(sd_angle), 2 ) * pow( rad_bbl_bdr_i, 2 );
      _sat_coverage[qp] = _initial_sat_coverage;
      _fract_active_faces[qp] = 1.;

      _bbl_bdr_3_old[qp] = bbl_bdr_2_i * 3./_initial_grain_radius*(1.-_initial_porosity) / 2.;
      _vcn_bdr_2_old[qp] = 0.;
      _rad_bbl_bdr_old[qp] = rad_bbl_bdr_i;
      _arp_bbl_bdr_old[qp] = M_PI * pow( sin(sd_angle), 2 ) * pow( rad_bbl_bdr_i, 2 );
      _GBCoverage_old[qp] = bbl_bdr_2_i * M_PI * pow( sin(sd_angle), 2 ) * pow( rad_bbl_bdr_i, 2 );
      _sat_coverage_old[qp] = _initial_sat_coverage;
      _fract_active_faces_old[qp] = 1.;
    }
    else
    {
      _bbl_bdr_2[qp] = 0.;
      _bbl_bdr_3[qp] = 0.;
      _rad_bbl_bdr[qp] = 0.;
      _arp_bbl_bdr[qp] = 0.;
      _vol_bbl_bdr[qp] = 0.;
      _GBCoverage[qp] = 0.;
      _sat_coverage[qp] = 0.;
      _fract_active_faces[qp] = 0.;

      _bbl_bdr_3_old[qp] = 0.;
      _vcn_bdr_2_old[qp] = 0.;
      _rad_bbl_bdr_old[qp] = 0.;
      _arp_bbl_bdr_old[qp] = 0.;
      _GBCoverage_old[qp] = 0.;
      _sat_coverage_old[qp] = 0.;
      _fract_active_faces_old[qp] = 0.;
    }

    _prs_bbl_bdr[qp] = 0.;
    _prseq_bbl_bdr[qp] = 0.;

    _deltav_v0_bd[qp] = 0.;
    _deltav_v0_sl[qp] = 0.;
    _deltav_v0_dn[qp] = 0.;
    _deltav_v0_swe[qp] = 0.;

    _deltav_v0_bd_old[qp] = 0.;
    _deltav_v0_sl_old[qp] = 0.;
    _deltav_v0_dn_old[qp] = 0.;
    _deltav_v0_swe_old[qp] = 0.;

    _fabrication_porosity[qp] = _initial_porosity;
    _gaseous_porosity[qp] = 0;
    _sintering_porosity[qp] = 0;

    if (_testing_output)
      (*_fract_FGR_analytical)[qp] = 0.;
  }
}


void Sifgrs::computeProperties()
{

  if ( _t_step == 0 || _t <= 0. )
  {
    for (unsigned qp = 0; qp < _qrule->n_points(); ++qp)
    {
      _actual_density[qp] = _initial_density;

      _linear_power[qp] = 0.;
      _linear_power_old[qp] = 0.;

      _burnup_loc[qp] = 0.;
      _burnup_loc_old[qp] = 0.;

      _gas_gen_3[qp] = 0.;
      _gas_grn_3[qp] = 0.;
      _gas_gbs_3[qp] = 0.;
      _gas_ath_3[qp] = 0.;
      _gas_rpr_3[qp] = 0.;
      _gas_hbs_3[qp] = 0.;
      _gas_hbr_3[qp] = 0.;
      _gas_bdr_2[qp] = 0.;
      _gas_bdr_3[qp] = 0.;
      _gas_thr_2[qp] = 0.;
      _gas_thr_3[qp] = 0.;
      _gas_trn_2[qp] = 0.;
      _gas_trn_3[qp] = 0.;
      _gas_rel_3[qp] = 0.;

      _gas_gen_3_old[qp] = 0.;
      _gas_grn_3_old[qp] = 0.;
      _gas_gbs_3_old[qp] = 0.;
      _gas_rpr_3_old[qp] = 0.;
      _gas_hbs_3_old[qp] = 0.;
      _gas_hbr_3_old[qp] = 0.;
      _gas_bdr_3_old[qp] = 0.;
      _gas_thr_3_old[qp] = 0.;
      _gas_trn_3_old[qp] = 0.;

      _crack_length[qp] = 0.;
      _crack_number[qp] = 0.;

      _g_0[qp] = 0.;
      _g_1[qp] = 0.;
      _g_2[qp] = 0.;
      _g_3[qp] = 0.;

      _g_0_old[qp] = 0.;
      _g_1_old[qp] = 0.;
      _g_2_old[qp] = 0.;
      _g_3_old[qp] = 0.;

      _bbl_grn_3[qp] = 0.;
      _rad_bbl_grn[qp] = rad_bbl_grn_i;
      _satom_fract_grn[qp] = 1.;

      _eff_diff_coeff[qp] = 1.e-35;
      _eff_diff_coeff_old[qp] = 1.e-35;

      _rampup_cond[qp] = false;

    if ( ! _skip_bdr_model )
    {
      _bbl_bdr_2[qp] = bbl_bdr_2_i;
      _bbl_bdr_3[qp] = bbl_bdr_2_i * 3./_initial_grain_radius*(1.-_initial_porosity) / 2.;
      _rad_bbl_bdr[qp] = rad_bbl_bdr_i;
      _arp_bbl_bdr[qp] = M_PI * pow( sin(sd_angle), 2 ) * pow( rad_bbl_bdr_i, 2 );
      _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow(rad_bbl_bdr_i,3);
      _GBCoverage[qp] = bbl_bdr_2_i * M_PI * pow( sin(sd_angle), 2 ) * pow( rad_bbl_bdr_i, 2 );
      _sat_coverage[qp] = _initial_sat_coverage;
      _fract_active_faces[qp] = 1.;

      _bbl_bdr_3_old[qp] = bbl_bdr_2_i * 3./_initial_grain_radius*(1.-_initial_porosity) / 2.;
      _vcn_bdr_2_old[qp] = 0.;
      _rad_bbl_bdr_old[qp] = rad_bbl_bdr_i;
      _arp_bbl_bdr_old[qp] = M_PI * pow( sin(sd_angle), 2 ) * pow( rad_bbl_bdr_i, 2 );
      _GBCoverage_old[qp] = bbl_bdr_2_i * M_PI * pow( sin(sd_angle), 2 ) * pow( rad_bbl_bdr_i, 2 );
      _sat_coverage_old[qp] = _initial_sat_coverage;
      _fract_active_faces_old[qp] = 1.;
    }
    else
    {
      _bbl_bdr_2[qp] = 0.;
      _bbl_bdr_3[qp] = 0.;
      _rad_bbl_bdr[qp] = 0.;
      _arp_bbl_bdr[qp] = 0.;
      _vol_bbl_bdr[qp] = 0.;
      _GBCoverage[qp] = 0.;
      _sat_coverage[qp] = 0.;
      _fract_active_faces[qp] = 0.;

      _bbl_bdr_3_old[qp] = 0.;
      _vcn_bdr_2_old[qp] = 0.;
      _rad_bbl_bdr_old[qp] = 0.;
      _arp_bbl_bdr_old[qp] = 0.;
      _GBCoverage_old[qp] = 0.;
      _sat_coverage_old[qp] = 0.;
      _fract_active_faces_old[qp] = 0.;
    }

      _prs_bbl_bdr[qp] = 0.;
      _prseq_bbl_bdr[qp] = 0.;

      _deltav_v0_bd[qp] = 0.;
      _deltav_v0_sl[qp] = 0.;
      _deltav_v0_dn[qp] = 0.;
      _deltav_v0_swe[qp] = 0.;

      _deltav_v0_bd_old[qp] = 0.;
      _deltav_v0_sl_old[qp] = 0.;
      _deltav_v0_dn_old[qp] = 0.;
      _deltav_v0_swe_old[qp] = 0.;

      _fabrication_porosity[qp] = _initial_porosity;
      _gaseous_porosity[qp] = 0;
      _sintering_porosity[qp] = 0;

      if (_testing_output)
      {
        (*_fract_FGR_analytical)[qp] = 0.;
      }
    }

    return;

  }

  for( unsigned qp=0; qp < _qrule->n_points(); ++qp )
  {
    const Real fission_rate = getFissionRate(qp);
    const Real fission_rate_old = getFissionRateOld(qp);


    if (_temp[qp] < 0.0)
    {
      if (n_processors()>1)
      {
        std::stringstream errorMsg;
        errorMsg << "Sifgrs: Negative temperature:"
                 << "\nElement:     " <<_current_elem->id()
                 << "\nqp:         " <<qp
                 << "\nTemperature: " << _temp[qp];
        mooseError(errorMsg.str());
      }
      else
      {
        std::stringstream errorMsg;
        errorMsg << "\nSifgrs: Negative temperature in element: "<<_current_elem->id();
        Moose::out<<errorMsg.str()<<std::endl<<std::endl;
        throw MooseException();
      }
    }

    if ( _rod_ave_lin_pow )
    {
      _linear_power[qp] = _rod_ave_lin_pow->value(_t,_q_point[qp]);
      if ( _axial_power_profile )
        _linear_power[qp] *= _axial_power_profile->value(_t,_q_point[qp]);
    }

    Real burnup = 0;
    if ( _compute_swelling || _hbs_model || _ramp_model)
    {
      burnup = getBurnup(qp);
      _burnup_loc[qp] = burnup * 9.5e+02;
    }

    if ( _has_grain_radius )
    {
      grn_radius = _grain_radius[qp];
      grn_radius_old = _grain_radius_old[qp];
    }

    grn_radius *= _grainradius_scalef;
    grn_radius_old *= _grainradius_scalef;

    const Real temperature = _temp[qp] * _temperature_scalef;
    const Real temperature_old = _temp_old[qp] * _temperature_scalef;

    /////////////////////////////////////////////////

    // Beginning of fission gas behavior calculation

    /////////////////////////////////////////////////

    _gas_grn_3[qp] = _gas_grn_3_old[qp];

    _g_0[qp] = _g_0_old[qp];
    _g_1[qp] = _g_1_old[qp];
    _g_2[qp] = _g_2_old[qp];
    _g_3[qp] = _g_3_old[qp];

    _gas_bdr_3[qp] = _gas_bdr_3_old[qp];

    _gas_gbs_3[qp] = _gas_gbs_3_old[qp];

    /* Grain boundary sweeping model */

    if ( _gbs_model )
    {
      // Compute Xe and Kr depeletion in growing grains caused by grain boundary sweeping

      Real gbs_fact(0.);

      const Real grnrad_cube_new = pow(grn_radius,    3);
      const Real grnrad_cube_old = pow(grn_radius_old,3);

      gbs_fact = ( grnrad_cube_new - grnrad_cube_old ) / grnrad_cube_new;
      if ( gbs_fact < 0. )  gbs_fact = 0.;

      Real dgas_gbs_3 = _gas_grn_3[qp] * gbs_fact;

      _gas_gbs_3[qp] += dgas_gbs_3;

      _gas_grn_3[qp] -= dgas_gbs_3;

      // correct factors for the diffusion algorithm

      _g_0[qp] *= (1.-gbs_fact);
      _g_1[qp] *= (1.-gbs_fact);
      _g_2[qp] *= (1.-gbs_fact);
      _g_3[qp] *= (1.-gbs_fact);

      // update grain-boundary gas concentration

      _gas_bdr_3[qp] += dgas_gbs_3;
    }

    Real gas_grn_3_ref = _gas_grn_3[qp];  // auxiliary

    //////////////////////////////////////////////////////////////////

    /* Intragranular model */

    // Gas generation

    const Real yield = fract_yield / avogadros_num;  // yield of fission gas (Xe + Kr) [mol]

    const Real gas_gen_rate_3     = fission_rate     * yield;
    const Real gas_gen_rate_old_3 = fission_rate_old * yield;

    const Real av_gas_gen_rate_3 = ( gas_gen_rate_3 + gas_gen_rate_old_3 ) * 0.5;  // average gas generation rate in the time step [mol/(m**3*s)]

    const Real dgas_gen_3 = av_gas_gen_rate_3 * _dt;

    _gas_gen_3[qp] = _gas_gen_3_old[qp] + dgas_gen_3;

    if ( _gas_gen_3[qp] < 0. )  _gas_gen_3[qp] = 0.;

    Real dgas_gen_3_eff = dgas_gen_3;

    // Athermal release

    _gas_ath_3[qp] = _gas_ath_3_old[qp];

    if ( _ath_model )
    {
      // Estimation of the number of cracks in the fuel pellet (correlation drawn from M. Oguma, NED 76, 35, 1983)

      if ( _linear_power[qp] < 6.e+03 )
        _crack_number[qp] = 0.;
      else
        _crack_number[qp] = _linear_power[qp] / 2.15e+03 - 1;

      // Assume cracks cross the external, brittle zone of the fuel pellet (D.R. Olander,
      // Fundamental aspects of nuclear reactor fuel elements, Berkeley, 1976)

      _crack_length[qp] = _pellet_brittle_zone->brittleZoneWidth(_pellet_id[qp]);

      const Real pellet_radius = _pellet_brittle_zone->pelletRadius();
      const Real pellet_height = _pellet_brittle_zone->pelletHeight();

      const Real pellet_volume = M_PI * pow(pellet_radius,2) * pellet_height;
      const Real geometrical_pellet_surface = 2. * M_PI * pellet_radius * ( pellet_height + pellet_radius );
      const Real cracked_pellet_surface = _crack_length[qp] * pellet_height * _crack_number[qp] * 2.;
      const Real total_pellet_surface = geometrical_pellet_surface + cracked_pellet_surface;

      // Athermal release model (B.J. Lewis, JNM 148, 28, 1987)

      const Real recoil_rate   = 0.25 * av_gas_gen_rate_3 * fissp_length * geometrical_pellet_surface;  // rate of gas release by recoil [mol/s]
      const Real knockout_rate = 0.5  * av_gas_gen_rate_3 * horder_knockon_range  * total_pellet_surface;  // rate of gas release by knockout [mol/s]

      const Real gas_ath_rate_3 = ( recoil_rate + knockout_rate ) / pellet_volume;  // athermal release rate [mol/(m**3*s)]

      const Real dgas_ath_3 = gas_ath_rate_3 * _dt;

      _gas_ath_3[qp] += dgas_ath_3;

      if ( _gas_ath_3[qp] < 0. )  _gas_ath_3[qp] = 0.;

      dgas_gen_3_eff  -= dgas_ath_3;
    }

    // Computation of the effective diffusion coefficient

    Real dterm_1(0.);
    Real dterm_2(0.);
    Real dterm_3(0.);
    Real d_satom(0.);

    switch( _diff_coeff_option )
    {
      case 0:
        // Single atom diffusion coefficient from (J.A. Turnbull et al., JNM 107, 168, 1982).
        // The radiation enhanced term, D2, is multiplied by a factor of 4 (J.A. Turnbull et al., Preston, England, September 18-22, 1988).
        // The purely fission rate dependent term, D3, is neglected.
        dterm_1 = 7.6e-10  * exp( - 4.8599e-19 / (boltzmann_const * temperature) );
        dterm_2 = 1.41e-25 * exp( - 1.9053e-19 / (boltzmann_const * temperature) ) * sqrt(fission_rate);
        d_satom = dterm_1 + 4.*dterm_2;
        break;

      case 1:
         // Single atom diffusion coefficient from atomistic modeling (D.A. Andersson et al., JNM, 2014)
        if ( temperature < 1671. )
          dterm_1 = 1.47e-05 * exp( - 7.91e-19 / (boltzmann_const * temperature) );
        else
          dterm_1 = 8.34e-12 * exp( - 4.69e-19 / (boltzmann_const * temperature) );
        if ( temperature < 1623. )
          dterm_2 = 1.47e-05 * exp( - 7.91e-19 / (boltzmann_const * temperature) ) * sqrt( fission_rate / 1.e+19 );
        else
          dterm_2 = 7.07e-15 * exp( - 2.96e-19 / (boltzmann_const * temperature) ) * sqrt( fission_rate / 1.e+19 );
        d_satom = dterm_1 + dterm_2;
        break;

      case 2:
        // Single atom diffusion coefficient from (J.A. Turnbull et al., JNM 107, 168, 1982)
        // The purely fission rate dependent term, D3, is considered.
        dterm_1 = 7.6e-10  * exp( - 4.8599e-19 / (boltzmann_const * temperature) );
        dterm_2 = 1.41e-25 * exp( - 1.9053e-19  / (boltzmann_const * temperature) ) * sqrt(fission_rate);
        dterm_3 = 2.e-40 * fission_rate;
        d_satom = dterm_1 + dterm_2 + dterm_3;
        break;

      case 3:
        // Single atom diffusion coefficient from (J.A. Turnbull et al., JNM 107, 168, 1982).
        // The purely fission rate dependent term, D3, is neglected.
        dterm_1 = 7.6e-10  * exp( - 4.8599e-19 / (boltzmann_const * temperature) );
        dterm_2 = 1.41e-25 * exp( - 1.9053e-19 / (boltzmann_const * temperature) ) * sqrt(fission_rate);
        d_satom = dterm_1 + dterm_2;
        break;
    }

    d_satom *= _igdiffcoeff_scalef;

    // Correction for the effects of trapping in and resolution from intragranular bubbles.
    // Correlations from (R.J. White, M.O. Tucker, JNM 118, 1, 1983) are used.

    Real resolution_par(0.); // resolution parameter [1/s]
    Real trapping_par(0.);   // trapping parameter [1/s]

    _rad_bbl_grn[qp] = 5.e-10*(1.+106.*exp(-8691.6/temperature));
    _bbl_grn_3[qp] = 1.52e+27/temperature - 3.3e+23;

    switch( _res_param_option )
    {
      case 0:
        // Resolution parameter according to White and Tucker, J. Nucl. Mater. 118, 1, 1983.
        resolution_par = 3.03 * fission_rate * M_PI * fissp_length * pow((_rad_bbl_grn[qp] + fissp_radius),2);
        trapping_par = 4. * M_PI * d_satom * _rad_bbl_grn[qp] * _bbl_grn_3[qp];
        break;

      case 1:
        // Compromise between the heterogeneous and the homogeneous resolution parameters (cf. Olander and Wongsawaeng, J. Nucl. Mater. 354, 94, 2006).
        // Thought for sensitivity analysis purposes.
        const Real bblrad_grn_const = 1.e-09;  // for consistency
        const Real bblndn3_grn_const = 7.e+23;
        resolution_par = 1.e-23 * fission_rate;
        trapping_par = 4. * M_PI * d_satom * bblrad_grn_const * bblndn3_grn_const;
        break;
    }

    resolution_par *= _resolutionp_scalef;

    _satom_fract_grn[qp] = resolution_par / ( resolution_par + trapping_par );

    _eff_diff_coeff[qp] = _satom_fract_grn[qp] * d_satom;

    if (_eff_diff_coeff[qp] < 1.e-35)  _eff_diff_coeff[qp] = 1.e-35;

    // Intragranular diffusion algorithm

    const Real deff = ( _eff_diff_coeff[qp] + _eff_diff_coeff_old[qp] ) * 0.5;  // average diffusion coefficient in the time step [m**2/s]

    // If testing_output = true, the analytical solution (M.V. Speight, Nuclear Science and Engineering 37, 180, 1969)
    // for the fractional release from the grains is calculated for the purpose of comparison with the numerical solution of Sifgr
    if (_testing_output)
    {
      int n = 1;
      int no = 51;
      (*_fract_FGR_analytical)[qp] = 0;
      while (no > n)
      {
        Real N = n;

        (*_fract_FGR_analytical)[qp] += (6*grn_radius*grn_radius) / (pow(M_PI,4) * deff * _t * pow(N,4))
          * (1 - exp(-deff*n*n*M_PI*M_PI*_t/(grn_radius*grn_radius)));

        ++n;
      }
      (*_fract_FGR_analytical)[qp] *= -1;
      (*_fract_FGR_analytical)[qp] +=  1;
    }

    // Transformed time step dtau

    const Real dtau = _dt * deff / pow(grn_radius,2);

    // Solve the intragranular diffusion equation using the algorithm in
    // (K. Forsberg, A.R. Massih, JNM 135, 140, 1985).
    // Grain-boundary resolution is neglected here (e.g., J. Rest, JNM 321, 305, 2003).

    Real f_terms[4];
    Real t1 (0.);
    for ( unsigned short int i=0; i<4; ++i )
    {
      Real arg = b_coeffs[i] * dtau;
      if ( arg < 4.e-4 )
        f_terms[i] = - arg;
      else if ( arg > 85. )
        f_terms[i] = - 1.;
      else
        f_terms[i] = exp( - arg ) - 1.;
      t1 += f_terms[i] * a_coeffs[i]/b_coeffs[i];
    }

    Real t2 (0.);
    if ( dtau < 0.1 )
      t2 = 2.25675833 * pow( dtau, 1.5 ) - 1.5 * pow( dtau, 2 );
    else
      t2 = dtau - 0.06159589 * ( 1. - exp( -9.86960440*dtau ) );

    const Real a2q = dgas_gen_3_eff / ( t2 - t1 );

    _g_0[qp] = _g_0[qp] + f_terms[0] * _g_0[qp] - f_terms[0] * a2q * a_coeffs[0]/b_coeffs[0];
    _g_1[qp] = _g_1[qp] + f_terms[1] * _g_1[qp] - f_terms[1] * a2q * a_coeffs[1]/b_coeffs[1];
    _g_2[qp] = _g_2[qp] + f_terms[2] * _g_2[qp] - f_terms[2] * a2q * a_coeffs[2]/b_coeffs[2];
    _g_3[qp] = _g_3[qp] + f_terms[3] * _g_3[qp] - f_terms[3] * a2q * a_coeffs[3]/b_coeffs[3];

    const Real gas_grn_3_new = _g_0[qp] +
                               _g_1[qp] +
                               _g_2[qp] +
                               _g_3[qp];

    const Real dgas_grn_3 = gas_grn_3_new - gas_grn_3_ref;

    const Real dgas_bdr_3 = dgas_gen_3_eff - dgas_grn_3;

    _gas_grn_3[qp] = gas_grn_3_new;

    if ( _gas_grn_3[qp] < 0. )  _gas_grn_3[qp] = 0.;

    _gas_bdr_3[qp] += dgas_bdr_3;

    if ( _gas_bdr_3[qp] < 0. )  _gas_bdr_3[qp] = 0.;

    //////////////////////////////////////////////////////////////////

    // Ramp release

    _gas_rpr_3[qp] = _gas_rpr_3_old[qp];

    if ( _ramp_model )
    {
      // Empirical model for (intragranular) ramp release according to
      // (Y.-H. Koo et al., Annals of Nuclear Energy 26, 47, 1999)

      // Check if power ramp conditions are fulfilled.

      const Real dlp_thres = 3500.;  // threshold linear power change [W/m]
      const Real lp_thres = 30000.;  // threshold linear power [W/m]
      const Real temp_thres = ( 1500. * ( 1. - _burnup_loc[qp] / 80. ) ) + 273.15;  // threshold temperature [K]

      const Real lp_change_rate = ( _linear_power[qp] - _linear_power_old[qp] ) / _dt;

      const unsigned short int num_steps = time_lp.size();

      bool same_step(false);

      if ( _t > time_lp[0] && _t <= time_lp[num_steps-1] )
      {
        for ( unsigned short int i=0; i<num_steps-1; ++i )
        {
          if ( _t > time_lp[i] && _t <= time_lp[i+1] )
          {
            if ( _axial_power_profile )  lp_input[i] *= _axial_power_profile->value(time_lp[i],_q_point[qp]);
            const Real dlp_ramp = std::max( (_linear_power[qp]-lp_input[i]), (_linear_power[qp]-_linear_power_old[qp]) );

            if ( ( dlp_ramp > dlp_thres          ) &&
                 ( _linear_power[qp] > lp_thres ) &&
                 ( temperature > temp_thres       ) &&
                 ( lp_change_rate > 1.           )    )

              _rampup_cond[qp] = true;

            else

              _rampup_cond[qp] = false;

            if ( _t-_dt > time_lp[i] && _t != time_lp[i+1] )
              same_step = true;
            else
              same_step = false;
          }
        }
      }

      else
      {
        _rampup_cond[qp] = false;
      }

      // During a power ramp, additional gas transfer from within grains to grain boundaries takes place
      // and is considered to occur instantaneously at the end of the ramp

      if ( _rampup_cond[qp] && !same_step )
      {
        Real rpr_fact(0.);

        rpr_fact = ( temperature - temp_thres ) / 1000.;
        if ( rpr_fact < 0.  )  rpr_fact = 0. ;
        if ( rpr_fact > 0.3 )  rpr_fact = 0.3;

        Real dgas_rpr_3 = _gas_grn_3[qp] * rpr_fact;

        _gas_rpr_3[qp] += dgas_rpr_3;

        _gas_grn_3[qp] -= dgas_rpr_3;

        // correct factors for the diffusion algorithm

        _g_0[qp] *= (1.-rpr_fact);
        _g_1[qp] *= (1.-rpr_fact);
        _g_2[qp] *= (1.-rpr_fact);
        _g_3[qp] *= (1.-rpr_fact);

        // update grain-boundary gas concentration

        _gas_bdr_3[qp] += dgas_rpr_3;
      }
    }

    //////////////////////////////////////////////////////////////////

    /* HBS model */

    _gas_hbs_3[qp] = _gas_hbs_3_old[qp];

    _gas_hbr_3[qp] = _gas_hbr_3_old[qp];

    if ( _hbs_model )
    {
      // Simple model for Xe and Kr depeletion in the high burnup structure (HBS)
      // from (K. Lassmann et al., JNM 226, 1, 1995)
      // A preliminary treatment of the gas release from the HBS porosity to the fuel rod free volume is included

      if ( _burnup_loc[qp] > buthr_hbs )
      {

        Real dburnup_loc(0.);

        if ( _burnup_loc_old[qp] > buthr_hbs )
          dburnup_loc = _burnup_loc[qp] - _burnup_loc_old[qp];
        else
          dburnup_loc = _burnup_loc[qp] - buthr_hbs;

        // calculate fission gas depletion

        const Real argmt = buconst * dburnup_loc;
        const Real depl_fact = + exp ( - argmt );

        const Real dgas_hbs_3 = _gas_grn_3[qp] * ( 1. - depl_fact );

        _gas_grn_3[qp] -= dgas_hbs_3;

        // correct factors for the diffusion algorithm

        _g_0[qp] *= depl_fact;
        _g_1[qp] *= depl_fact;
        _g_2[qp] *= depl_fact;
        _g_3[qp] *= depl_fact;

        // update fission gas in HBS porosity

        _gas_hbs_3[qp] += dgas_hbs_3;

        //////////////////////////////////////////////////////////////////

        /* compute FGR from the HBS (preliminary!) */

        if ( _burnup_loc[qp] > _hbs_release_burnup )
        {

          Real rel_fact(0.);

          if ( _burnup_loc_old[qp] > _hbs_release_burnup )
            rel_fact = 1.;
          else
            rel_fact = ( _burnup_loc[qp] - _hbs_release_burnup ) / ( _burnup_loc[qp] - _burnup_loc_old[qp] );

          const Real dgas_hbr_3 = dgas_hbs_3 * rel_fact;

          _gas_hbr_3[qp] += dgas_hbr_3;

          if ( _gas_hbr_3[qp] < 0. )  _gas_hbr_3[qp] = 0.;

          _gas_hbs_3[qp] -= dgas_hbr_3;
        }
      }
    }

    //////////////////////////////////////////////////////////////////

    /* Grain boundary model */

    const Real srf_vol_ratio = 3. / grn_radius * ( 1. - _initial_porosity );  // The evolution of grain-boundary porosity is not considered
                                                                              // in the calculation of the grain surface to fuel volume ratio.

    if ( ! _skip_bdr_model )
    {

      _vcn_bdr_2[qp] = _vcn_bdr_2_old[qp];

      _bbl_bdr_3[qp] = _bbl_bdr_3_old[qp];
      _bbl_bdr_2[qp] = _bbl_bdr_3[qp] / srf_vol_ratio * 2.;
      if ( _bbl_bdr_2[qp] > bbl_bdr_2_i )  _bbl_bdr_2[qp] = bbl_bdr_2_i;

      _gas_thr_3[qp] = _gas_thr_3_old[qp];
      _gas_thr_2[qp] = _gas_thr_3[qp] / srf_vol_ratio;

      _gas_trn_3[qp] = _gas_trn_3_old[qp];
      _gas_trn_2[qp] = _gas_trn_3[qp] / srf_vol_ratio;

      const Real temp_av = ( temperature + temperature_old ) * 0.5;

      _gas_bdr_2[qp] = _gas_bdr_3[qp] / srf_vol_ratio;

      _atm_bbl_bdr[qp] = _gas_bdr_2[qp] * avogadros_num / ( _bbl_bdr_2[qp] / 2. );
      // The factor of 1/2 is introduced because each intergranular bubble is shared by two
      // neighboring grains, and accordingly bbl_bdr_2 is defined per unit granular interface
      // (granular interface = 1/2 grain surface), while gas_bdr_2 is per unit grain surface.

      if ( _atm_bbl_bdr[qp] > 0. )
      {
        /* Compute grain-boundary bubble radius based on (M.V Speight, W. Beere, Metal Science 9, 190, 1975; R.J. White, JNM 325, 61, 2004) */

        // Diffusion coefficient of vacancies in grain boundary [m**2/s].
        Real d_grb = 8.86e-06 * exp ( - act_energy / ( boltzmann_const * temp_av) );  // Correlation drawn from Fig. 14 in (R.J. White, JNM 325, 61, 2004).
        d_grb *= _gbdiffcoeff_scalef;

        Real fract_coverage = _GBCoverage_old[qp];
        if ( fract_coverage < 1.E-02 )  fract_coverage = 1.E-02;
        if ( fract_coverage > 0.9    )  fract_coverage = 0.9; // may be useful for testing
        const Real s_param = - ( ( 3. - fract_coverage ) * ( 1. - fract_coverage ) + 2. * log ( fract_coverage ) ) / 4.;

        _vcn_bbl_bdr[qp] = _vcn_bdr_2[qp] / _bbl_bdr_2[qp];

        Real vcn_per_atm(0.);
        if ( _rad_bbl_bdr_old[qp] > rad_bbl_bdr_i )
        {
          vcn_per_atm = _vcn_bbl_bdr[qp] / _atm_bbl_bdr[qp];
          if (vcn_per_atm < 1.e-01)  vcn_per_atm = 1.e-01;
        }
        else
          vcn_per_atm = 1.e-01;

        _prs_bbl_bdr[qp] = boltzmann_const * temperature / ( vcn_volume * vcn_per_atm );

        Real ext_pressure(0.);

        if ( _has_hydrostatic_stress )
        {
          if ( _hydrostatic_stress[qp] < 0.)
            ext_pressure = - _hydrostatic_stress[qp];
          else
            ext_pressure = 0.;
        }
        else
        {
          ext_pressure = _hydrostatic_stress_const;
          if ( ext_pressure < 0. )  ext_pressure = 0.;
        }

        _prseq_bbl_bdr[qp] = 2.*surface_tension/_rad_bbl_bdr_old[qp] + ext_pressure;

        const Real dvcn_bbl_bdr = 2. * M_PI * d_grb * grb_thickness / ( boltzmann_const * temp_av * s_param ) *
                                  ( _prs_bbl_bdr[qp] - _prseq_bbl_bdr[qp] ) * _dt;

        _vcn_bbl_bdr[qp] += dvcn_bbl_bdr;

        const Real voleq_bbl_bdr = _atm_bbl_bdr[qp] * ( vanderwaals_b/avogadros_num + boltzmann_const * temperature / _prseq_bbl_bdr[qp] );  // equilibrium bubble volume [m**3]
        const Real vcneq_bbl_bdr = ( voleq_bbl_bdr - _atm_bbl_bdr[qp] * vanderwaals_b/avogadros_num ) / vcn_volume;  // equilibrium number of vacancies per bubble

        if ( ( _prs_bbl_bdr[qp] > _prseq_bbl_bdr[qp] && _vcn_bbl_bdr[qp] > vcneq_bbl_bdr ) ||
             ( _prs_bbl_bdr[qp] < _prseq_bbl_bdr[qp] && _vcn_bbl_bdr[qp] < vcneq_bbl_bdr )    ) // Going beyond the equilibrium size is unphysical (possible time step effect)
          _vcn_bbl_bdr[qp] = vcneq_bbl_bdr;

        vcn_per_atm = _vcn_bbl_bdr[qp] / _atm_bbl_bdr[qp];

        _vcn_bdr_2[qp] = _bbl_bdr_2[qp] * _vcn_bbl_bdr[qp];

        _vol_bbl_bdr[qp] = _atm_bbl_bdr[qp] * vanderwaals_b/avogadros_num + _vcn_bbl_bdr[qp] * vcn_volume;

        _rad_bbl_bdr[qp] = pow ( _vol_bbl_bdr[qp] / 1.33333333 / M_PI / geom_fact, 0.33333333 );
        if ( _rad_bbl_bdr[qp] < rad_bbl_bdr_i )
        {
          _rad_bbl_bdr[qp] = rad_bbl_bdr_i;
          _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow( _rad_bbl_bdr[qp], 3 );
        }

        _arp_bbl_bdr[qp] = M_PI * pow( sin(sd_angle), 2 ) * pow( _rad_bbl_bdr[qp], 2 );

        _GBCoverage[qp] = _bbl_bdr_2[qp] * _arp_bbl_bdr[qp];

        const Real darp_bbl_bdr = _arp_bbl_bdr[qp] - _arp_bbl_bdr_old[qp];

        //////////////////////////////////////////////////////////////////

        if ( darp_bbl_bdr > 0. )
        {
          // Bubble coalescence model based on (R.J. White, JNM 325, 61, 2004)

          _bbl_bdr_2[qp] = _bbl_bdr_2[qp] / ( 1. + 2. * _bbl_bdr_2[qp] * darp_bbl_bdr );
          if ( _bbl_bdr_2[qp] < 1.e+10 )  _bbl_bdr_2[qp] = 1.e+10;

          _atm_bbl_bdr[qp] = _gas_bdr_2[qp] * avogadros_num / ( _bbl_bdr_2[qp] / 2. );
          if ( _atm_bbl_bdr[qp] < 0. )  _atm_bbl_bdr[qp] = 0.;

          _vcn_bbl_bdr[qp] = _vcn_bdr_2[qp] / _bbl_bdr_2[qp];

          _vol_bbl_bdr[qp] = _atm_bbl_bdr[qp] * vanderwaals_b/avogadros_num + _vcn_bbl_bdr[qp] * vcn_volume;

          _rad_bbl_bdr[qp] = pow ( _vol_bbl_bdr[qp] / 1.33333333 / M_PI / geom_fact, 0.33333333 );
          if ( _rad_bbl_bdr[qp] < rad_bbl_bdr_i )
          {
            _rad_bbl_bdr[qp] = rad_bbl_bdr_i;
            _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow( _rad_bbl_bdr[qp], 3 );
          }

          _arp_bbl_bdr[qp] = M_PI * pow( sin(sd_angle), 2 ) * pow( _rad_bbl_bdr[qp], 2 );

          _GBCoverage[qp] = _bbl_bdr_2[qp] * _arp_bbl_bdr[qp];

          _bbl_bdr_3[qp] = _bbl_bdr_2[qp] * srf_vol_ratio / 2.;
        }

      //////////////////////////////////////////////////////////////////

      /* Thermal fission gas release model */

      _sat_coverage[qp] = _sat_coverage_old[qp];

      if ( _rad_bbl_bdr[qp] > rad_bbl_bdr_i )
      {

        if ( _GBCoverage[qp] > _sat_coverage[qp] )
        {
          _GBCoverage[qp]  = _sat_coverage[qp];

          _arp_bbl_bdr[qp] = _GBCoverage[qp] / _bbl_bdr_2[qp];
          _rad_bbl_bdr[qp] = pow( ( _arp_bbl_bdr[qp] / ( M_PI * pow(sin(sd_angle),2) ) ), 0.5 );
          if ( _rad_bbl_bdr[qp] < rad_bbl_bdr_i ) _rad_bbl_bdr[qp] = rad_bbl_bdr_i;
            _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow(_rad_bbl_bdr[qp],3);

          _atm_bbl_bdr[qp] = _vol_bbl_bdr[qp] / ( vanderwaals_b/avogadros_num + vcn_volume * vcn_per_atm);
          _vcn_bbl_bdr[qp] = vcn_per_atm * _atm_bbl_bdr[qp];

          _vcn_bdr_2[qp] = _bbl_bdr_2[qp] * _vcn_bbl_bdr[qp];

          const Real dgas_thr_2 = _gas_bdr_2[qp] - ( _atm_bbl_bdr[qp] / avogadros_num * ( _bbl_bdr_2[qp] / 2. ) );

          _gas_bdr_2[qp] -= dgas_thr_2;
          _gas_bdr_3[qp] = _gas_bdr_2[qp] * srf_vol_ratio;

          _gas_thr_2[qp] += dgas_thr_2;
          if ( _gas_thr_2[qp] < 0. )  _gas_thr_2[qp] = 0.;
          _gas_thr_3[qp] = _gas_thr_2[qp] * srf_vol_ratio;

          if ( _rad_bbl_bdr[qp] < rad_bbl_bdr_i && _transient_option != 0 )
          {
            _rad_bbl_bdr[qp] = rad_bbl_bdr_i;
            _arp_bbl_bdr[qp] = M_PI * pow( sin(sd_angle), 2 ) * pow( _rad_bbl_bdr[qp], 2 );
            _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow(_rad_bbl_bdr[qp],3);
            _GBCoverage[qp] = _bbl_bdr_2[qp] * _arp_bbl_bdr[qp];
            if ( _sat_coverage[qp] < _GBCoverage[qp] )
                 _sat_coverage[qp] = _bbl_bdr_2[qp] * _arp_bbl_bdr[qp];
          }
        }
      }

        //////////////////////////////////////////////////////////////////

        /* Transient fission gas release model */

        if ( _transient_option != 0 )
        {
          _fract_active_faces[qp] = _fract_active_faces_old[qp];

          if ( _rad_bbl_bdr[qp] > rad_bbl_bdr_i )
          {
            const Real dtemp  = temperature - temperature_old;

            if ( std::abs(dtemp) >= 1.e-03 )
            {
              Real sigmoid_funct(1.);
              Real sigmoid_funct_old(1.);

              /* Simple sigmoid */

              if ( _transient_option == 1 )
              {
                const Real arg_exp  = ( temperature - temp_center ) / temp_span;
                const Real exp_term = exp( - arg_exp );

                const Real arg_exp_old  = ( temperature_old - temp_center ) / temp_span;
                const Real exp_term_old = exp( - arg_exp_old );

                if ( dtemp > 0. ) // Heating transient
                {
                  sigmoid_funct     = 1. - 1. / ( 1. + exp_term     );
                  sigmoid_funct_old = 1. - 1. / ( 1. + exp_term_old );
                }

                else              // Cooling transient
                {
                  sigmoid_funct     = 1. / ( 1. + exp_term     );
                  sigmoid_funct_old = 1. / ( 1. + exp_term_old );
                }
              }

              /* Generalized sigmoid */

              if ( _transient_option == 2 )
              {
                const Real B_param = 5.;
                const Real Q_param = 33.;

                if ( dtemp > 0. ) // Heating transient
                {
                  const Real arg_heat = ( temp_center - temperature ) / B_param;
                  const Real exp_heat = exp( - arg_heat );

                  const Real arg_heat_old = ( temp_center - temperature_old ) / B_param;
                  const Real exp_heat_old = exp( - arg_heat_old );

                  sigmoid_funct     = 1. / pow( ( 1. + Q_param * exp_heat     ), 1./Q_param );
                  sigmoid_funct_old = 1. / pow( ( 1. + Q_param * exp_heat_old ), 1./Q_param );
                }

                else              // Cooling transient
                {
                  const Real arg_cool = ( temperature - temp_center ) / B_param;
                  const Real exp_cool = exp( - arg_cool );

                  const Real arg_cool_old = ( temperature_old - temp_center ) / B_param;
                  const Real exp_cool_old = exp( - arg_cool_old );

                  sigmoid_funct     = 1. / pow( ( 1. + Q_param * exp_cool     ), 1./Q_param );
                  sigmoid_funct_old = 1. / pow( ( 1. + Q_param * exp_cool_old ), 1./Q_param );
                }
              }
              const Real dsigmoid_funct = sigmoid_funct - sigmoid_funct_old;

              _fract_active_faces[qp] = _fract_active_faces_old[qp] * exp( dsigmoid_funct );

              _GBCoverage[qp]   *= _fract_active_faces[qp] / _fract_active_faces_old[qp];
              _sat_coverage[qp] *= _fract_active_faces[qp] / _fract_active_faces_old[qp];

              _arp_bbl_bdr[qp] = _GBCoverage[qp] / _bbl_bdr_2[qp];
              _rad_bbl_bdr[qp] = sqrt( _arp_bbl_bdr[qp] / ( M_PI * pow(sin(sd_angle),2.) ) );
              _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow(_rad_bbl_bdr[qp],3.);

              _atm_bbl_bdr[qp] *= pow( ( _fract_active_faces[qp] / _fract_active_faces_old[qp] ), 1.5 ); // valid for spherical projected
              _vcn_bbl_bdr[qp] = vcn_per_atm * _atm_bbl_bdr[qp];                                         // bubbles ( A ~ R^2, V ~ R^3 )

              _vcn_bdr_2[qp] = _bbl_bdr_2[qp] * _vcn_bbl_bdr[qp];

              const Real dgas_trn_2 = _gas_bdr_2[qp] - ( _atm_bbl_bdr[qp] / avogadros_num * ( _bbl_bdr_2[qp] / 2. ) );

              _gas_bdr_2[qp] -= dgas_trn_2;
              _gas_bdr_3[qp] = _gas_bdr_2[qp] * srf_vol_ratio;

              _gas_trn_2[qp] += dgas_trn_2;
              if ( _gas_trn_2[qp] < 0. )  _gas_trn_2[qp] = 0.;
              _gas_trn_3[qp] = _gas_trn_2[qp] * srf_vol_ratio;

              if ( _rad_bbl_bdr[qp] < rad_bbl_bdr_i )
              {
                _rad_bbl_bdr[qp] = rad_bbl_bdr_i;
                _arp_bbl_bdr[qp] = M_PI * pow( sin(sd_angle), 2. ) * pow( _rad_bbl_bdr[qp], 2. );
                _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow(_rad_bbl_bdr[qp], 3. );
                _GBCoverage[qp] = _bbl_bdr_2[qp] * _arp_bbl_bdr[qp];
                if ( _sat_coverage[qp] < _GBCoverage[qp] )
                  _sat_coverage[qp] = _bbl_bdr_2[qp] * _arp_bbl_bdr[qp];
              }
            }

            /* Healing */

            if ( _sat_coverage[qp] < _initial_sat_coverage )
            {
              const Real healing_burnup = 1.; // (MWd/kgU) Hering, 1982, JNM

              const Real dhealed_fraction = 1. - exp( - ( _burnup_loc[qp] - _burnup_loc_old[qp] ) / healing_burnup );

              _sat_coverage[qp] += ( _initial_sat_coverage - _sat_coverage[qp] ) * dhealed_fraction;
            }
          }
        }
      }

      //////////////////////////////////////////////////////////////////
      else
      {
        _vcn_bbl_bdr[qp] = 0.;

        _prs_bbl_bdr[qp] = 0.;
        _prseq_bbl_bdr[qp] = 0.;

        _rad_bbl_bdr[qp] = rad_bbl_bdr_i;
        _vol_bbl_bdr[qp] = 1.33333333 * M_PI * geom_fact * pow( _rad_bbl_bdr[qp], 3 );
        _arp_bbl_bdr[qp] = M_PI * pow( sin(sd_angle), 2 ) * pow( _rad_bbl_bdr[qp], 2 );
        _GBCoverage[qp] = _bbl_bdr_2[qp] * _arp_bbl_bdr[qp];
      }

      _gas_rel_3[qp] = _gas_thr_3[qp] + _gas_ath_3[qp] + _gas_hbr_3[qp] + _gas_trn_3[qp];
    }

    else
    {
      const Real dgas_thr_3 = _gas_bdr_3[qp];
      _gas_thr_3[qp] = _gas_thr_3_old[qp] + dgas_thr_3;
      if ( _gas_thr_3[qp] < 0. )  _gas_thr_3[qp] = 0.;
      _gas_bdr_3[qp] = 0.;

      _gas_thr_2[qp] = _gas_thr_3[qp] / srf_vol_ratio;
      _gas_bdr_2[qp] = 0.;

      _bbl_bdr_2[qp] = 0.;
      _bbl_bdr_3[qp] = 0.;
      _vcn_bdr_2[qp] = 0.;
      _atm_bbl_bdr[qp] = 0.;
      _vcn_bbl_bdr[qp] = 0.;
      _rad_bbl_bdr[qp] = 0.;
      _arp_bbl_bdr[qp] = 0.;
      _vol_bbl_bdr[qp] = 0.;
      _GBCoverage[qp] = 0.;
      _sat_coverage[qp] = 0.;
      _fract_active_faces[qp] = 0.;

      _gas_rel_3[qp] = _gas_thr_3[qp] + _gas_ath_3[qp] + _gas_hbr_3[qp];
    }

    //////////////////////////////////////////////////////////////////

    /* Swelling model */

    _deltav_v0_bd[qp] = _deltav_v0_bd_old[qp];
    _deltav_v0_sl[qp] = _deltav_v0_sl_old[qp];
    _deltav_v0_dn[qp] = _deltav_v0_dn_old[qp];
    _deltav_v0_swe[qp] = _deltav_v0_swe_old[qp];

    if ( _compute_swelling )
    {
      /* Fission gas swelling */

      // Fractional volume grain-boundary fission gas swelling [/]
      _deltav_v0_bd[qp] = _bbl_bdr_3[qp] * _vol_bbl_bdr[qp];
      if ( _deltav_v0_bd[qp] < 0. )  _deltav_v0_bd[qp] = 0.;
      ddeltav_v0_bd = _deltav_v0_bd[qp] - _deltav_v0_bd_old[qp];

      //////////////////////////////////////////////////////////////////

      const Real fuel_density = _actual_density[qp];

      /* Solid fission product swelling */

      // Empirical model from MATPRO
      VSwellingUO2::solidSwelling( _solid_swelling_factor, fuel_density, burnup, ddeltav_v0_sl, dddeltav_v0_sl_dT );

      // Fractional volume solid swelling [/]
      _deltav_v0_sl[qp] = ddeltav_v0_sl;
      if ( _deltav_v0_sl[qp] < 0. )  _deltav_v0_sl[qp] = 0.;

      /* Densification */

      VSwellingUO2::densification( _total_densification, _end_densification_burnup, false, temperature, burnup, ddeltav_v0_dn, dddeltav_v0_dn_dT );

      // Fractional volume densification [/]
      _deltav_v0_dn[qp] =  ddeltav_v0_dn;
      if ( _deltav_v0_dn[qp] > 0. )  _deltav_v0_dn[qp] = 0.;

      /* Fractional volume total swelling [/] */

      _deltav_v0_swe[qp] = _deltav_v0_bd[qp] + _deltav_v0_sl[qp] + _deltav_v0_dn[qp];
    }

    //////////////////////////////////////////////////////////////////

    /* Contributions to fuel porosity (used in PorosityAuxUO2) */

    const Real volume_factor = _actual_density[qp] / _initial_density;

    _fabrication_porosity[qp] = _initial_porosity * volume_factor;
    if ( _fabrication_porosity[qp] < 0. ) _fabrication_porosity[qp] = 0.;

    _gaseous_porosity[qp] = _deltav_v0_bd[qp] * volume_factor;
    if ( _gaseous_porosity[qp] < 0. ) _gaseous_porosity[qp] = 0.;

    _sintering_porosity[qp] = _deltav_v0_dn[qp] * volume_factor;
    if ( _sintering_porosity[qp] > 0. ) _sintering_porosity[qp] = 0.;
    if ( _sintering_porosity[qp] < - _fabrication_porosity[qp] )  _sintering_porosity[qp] = - _fabrication_porosity[qp];
  }
}

//////////////////////////////////////////////////////////////////

bool Sifgrs::parseNextLineReals(std::ifstream & ifs, std::vector<Real> & myvec)
{
  std::string line;
  myvec.clear();
  bool gotline(false);
  if (getline(ifs,line))
  {
    gotline=true;

    //Replace all commas with spaces
    while(size_t pos=line.find(','))
    {
      if (pos == line.npos)
        break;
      line.replace(pos,1,1,' ');
    }

    //Harvest floats separated by whitespace
    std::istringstream iss(line);
    Real f;
    while (iss>>f)
    {
      myvec.push_back(f);
    }
  }
  return gotline;
}

void Sifgrs::parseRows( std::vector<Real> & x, std::vector<Real> & y )
{
  std::ifstream file(_file_name.c_str());
  if (!file.good())
    mooseError("Error opening file '" + _file_name + "' from Sifgrs.");
  std::string line;

  while(parseNextLineReals(file, x))
  {
    if (x.size() >0)
      break;
  }

  if (x.size() == 0)
    mooseError("From Sifgrs: File '" + _file_name + "' contains no data.");

  while(parseNextLineReals(file, y))
  {
    if (y.size() >0)
      break;
  }

  if (y.size() == 0)
    mooseError("From Sifgrs: File '" + _file_name + "' contains no y data.");
  else if (y.size() != x.size())
    mooseError("From Sifgrs: Lengths of x and y data do not match in file '" + _file_name + "'");

  std::vector<Real> scratch;
  while(parseNextLineReals(file, scratch)){
    if (scratch.size() > 0)
      mooseError("From Sifgrs: Read more than two rows of data from file '" + _file_name + "'.  Did you mean to use \"format = columns\"?");
  }
}

void Sifgrs::parseColumns( std::vector<Real> & x, std::vector<Real> & y )
{
  std::ifstream file(_file_name.c_str());
  if (!file.good())
    mooseError("Error opening file '" + _file_name + "' from Sifgrs.");
  std::string line;

  std::vector<Real> scratch;
  while(parseNextLineReals(file, scratch))
  {
    if (scratch.size() > 0){
      if (scratch.size() != 2)
        mooseError("From Sifgrs: Read more than 2 columns of data from file '" + _file_name + "'.  Did you mean to use \"format = rows\"?");
      x.push_back(scratch[0]);
      y.push_back(scratch[1]);
    }
  }
}

Real
Sifgrs::getFissionRate(unsigned qp)
{
  return _has_fission_rate ? _fission_rate[qp] : _burnup_function->fissionRate( _q_point[qp] );
}

Real
Sifgrs::getFissionRateOld(unsigned qp)
{
  return _has_fission_rate ? _fission_rate_old[qp] : _burnup_function->fissionRateOld( _q_point[qp] );
}

Real
Sifgrs::getBurnup(unsigned qp)
{
  return _has_burnup ? _burnup[qp] : _burnup_function->burnup( _q_point[qp] );
}

Real
Sifgrs::getBurnupOld(unsigned qp)
{
  return _has_burnup ? _burnup_old[qp] : _burnup_function->burnupOld( _q_point[qp] );
}
