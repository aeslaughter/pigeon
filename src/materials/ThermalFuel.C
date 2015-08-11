#include "ThermalFuel.h"

#include "BurnupFunction.h"

/* This kernel computes the thermophysical material properties of UO@ and/or MOX found in:

   0)  Duriez-Lucuta model: recommended and developped in J.J. Carbajo et al.,
       A review of the thermophysical properties of MOX and UO2 fuels, Journal
       of Nuclear Materials, Vol. 299, p181-198 (2001)

   1)  Amaya-Lucuta model: uses Fink for the base and then mods for unirradiated MOX thermal
       conductivity found in:
       M. AMAYA et al., Thermal conductivity evaluation of high burnup mixed-
       oxide (MOX) fuel pellet, Journal of Nuclear Materials, Vol. 414, p303-
       308, (2011).
       Amaya needs an expression for unirradiated conductivity of UO2. Fink model
       used in: J.K. Fink, Therophysical properties of uranium dioxide, Journal
       of Nuclear Materials, Vol. 279, p1-18 (2000)
       Lucuta corrections accounting for burnup found in Carbajo previous paper.

    2)  Fink-Lucuta model: J. K. Fink, Thermophysical Properties of Uranium Dioxide, Journal of Nuclear
        Materials, Vol. 279, p. 1-18, 2000 and P. G. Lucuta et al., A Pragmatic Approach to Modeling
        Thermal Conductivity of Irradiated UO2 Fuel: Review and Recommendations, Journal of Nuclear
        Materials, Vol. 232, p. 166-180, 1996.

    3)  Halden model: Documented in D. D. Lanning, C. E. Beyer, K.J.Geelhood, FRAPCON-3 Updates, Including
        Mixed-Oxide Fuel Properties, NUREG/CR-6534, Vol. 4 PNNL–11513, pg 3.4, 2005.

    4)  NFIR model: Documented in A. Marion (NEI) letter dated June 13, 2006 to H. N. Berkow (USNRC/NRR), 'Safety Evaluation by the Office of Nuclear Reactor Regulation of Electric Power Research Institute (EPRI) Topical
        Report TR-1002865, “Topical Report on Reactivity Initiated Accidents: Bases for RIA Fuel
        Rod Failures and Core Coolability Criteria”', Response to Requests for Additional Information
        (RAI’s) NRR Safety Evaluation Report – Project No. 689, Appendix C-1.A, pp C-1-15-16.  Obtained from
        http://pbadupws.nrc.gov/docs/ML0616/ML061650107.pdf

 5)  Modified NFI model:  Original NFI model documented in Ohira, K., and N. Itagaki. “Thermal Conductivity Measurements of High Burnup UO2 Pellet and a Benchmark Calculation of Fuel Center Temperature,” in Proceedings of the ANS International Topical Meeting on LWR Fuel Performance, pp. 541-549. March 2-6, 1997, Portland, Oregon, 1997 and Ogata, K., T. Karasawa, T. Anegawa, S. Koizumi, and K. Ito. “Japanese BWR Fuel Performance and Recent R& D Activities,” in ANS/ENS Topical Meeting on LWR Fuel Performance, April 10-13, 2000, Park City, Utah, pp. 108-117. 2000.  Modifications documented in D. D. Lanning, C. E. Beyer, K.J.Geelhood, FRAPCON-3 Updates, Including Mixed-Oxide Fuel Properties, NUREG/CR-6534, Vol. 4 PNNL–11513, pg 2.1 - 2.4, 2005.

   In absence of more precise information, it is assumed that the specific heat
   of MOX remains the same as UO2. Specific heat is computed using Fink model.

*/

template<>
InputParameters validParams<ThermalFuel>()
{
  InputParameters params = validParams<Material>();

  params.addCoupledVar("temp", "Coupled Temperature");
  params.addCoupledVar("burnup", "Coupled Burnup Rate");
  params.addParam<FunctionName>("burnup_function", "Burnup function");
  params.addCoupledVar("porosity", "Coupled Porosity");

  params.addParam<bool>("porosity_material", false, "Whether a material property for porosity is supplied");

  // if coupled porosity is not provided, the initial value is used
  params.addParam<Real>("initial_porosity", 0.05, "Initial porosity");

  params.addParam<Real>("oxy_to_metal_ratio", 2.0, "Deviation from stoichiometry");
  params.addParam<Real>("Pu_content", 0.00, "Weight fraction of Pu in MOX fuel (typically ~0.07)");
  params.addParam<Real>("Gd_content", 0.0, "Weight fraction of gadolinium in fuel");
  params.addRequiredParam<int>("model","Thermal conductivity models: 0==Duriez (MOX), 1==Amaya (MOX), 2==Fink-Lucuta (UO2), 3==Halden (UO2 (w/wo Gd) or MOX), 4==NFIR (UO2), or 5 == Modified NFI (UO2 w/wo Gd)");

  return params;
}

ThermalFuel::ThermalFuel(const InputParameters & parameters) :
  Material(parameters),

  _has_temp(isCoupled("temp")),
  _temp(_has_temp ? coupledValue("temp") : _zero),
  _grad_temp(_has_temp ? coupledGradient("temp") : _grad_zero),

  _has_burnup(isCoupled("burnup")),
  _burnup(_has_burnup ? coupledValue("burnup") : _zero),
  _burnup_function( isParamValid("burnup_function") ?
                    dynamic_cast<BurnupFunction*>(&getFunction("burnup_function")) : NULL ),

  _has_porosity(isCoupled("porosity")),
  _porosity(_has_porosity ? coupledValue("porosity") : _zero),

  _porosity_material(getParam<bool>("porosity_material")),
  _porosity_mat(_porosity_material ? &getMaterialProperty<Real>("porosity") : NULL),

  _thermal_conductivity(declareProperty<Real>("thermal_conductivity")),
  _thermal_conductivity_dT(declareProperty<Real>("thermal_conductivity_dT")),
  _specific_heat(declareProperty<Real>("specific_heat")),

  _initial_porosity(getParam<Real>("initial_porosity")),
  _stoich_dev(2.0 - getParam<Real>("oxy_to_metal_ratio")),  //deviation from stoichiometry
  _Pu_content(getParam<Real>("Pu_content")),                // MOX
  _Gd_content(getParam<Real>("Gd_content")),              // burnable poison
  _model(getParam<int>("model"))

{
  if (_has_porosity && _porosity_material)
  {
    mooseError("Porosity can be specified as a material property or coupled variable but not both");
  }

  if (!_has_burnup && !_burnup_function)
  {
    mooseError("Must supply burnup or burnup_function in ThermalFuel");
  }
}


Real
ThermalFuel::computeSpecificHeat(const Real temp)
{
  const Real tnd  = temp/1000;
  const Real tnd2 = tnd  * tnd;
  const Real tnd3 = tnd2 * tnd;
  const Real tnd4 = tnd3 * tnd;

  const Real cpm = 52.1743 + 87.95*tnd - 84.2411*tnd2 + 31.542*tnd3 - 2.6334*tnd4
                   - 0.71391/tnd2; // eqn 4 (Fink)

  // Specific heat with molecular weight conversion
  return cpm/0.27;
}

// model 1 (base) and model 2
void
ThermalFuel::computeFinkUnirradThCond(const Real temp, Real & cond0, Real & cond0_dt)
{
  const Real tnd = temp/1000.   ;
  const Real tnd2 = tnd * tnd        ;
  const Real tnd2p5 = pow(tnd,2.5e0) ;

  // Temperature dependent thermal conductivity, corrected to full density
  const Real pfact = 1. / (1. - (2.6-0.5*tnd)*0.05); // eqn 18 (Fink)
  const Real pfact_dT = -2.5e-5/((0.87-2.5e-5*temp)*(0.87-2.5e-5*temp));
  const Real d(7.5408 + 17.692*tnd + 3.6142*tnd2);
  const Real exp1( exp(-16.35/tnd) );
  const Real term = 100. / d + (6400./tnd2p5)*exp1;
  const Real term_dT = (-100./(d*d)*(17.692+2*3.6142*tnd) +
                        -2.5*6400*std::pow(tnd,-3.5)*exp1 +
                        6400/tnd2p5*exp1*16.35/(tnd*tnd)) / 1000.0;
  cond0 = term * pfact; // eqns 19*18 (Fink)
  cond0_dt = term_dT * pfact + term * pfact_dT;
}

// model 0
void
ThermalFuel::computeDuriezUnirradThCond(const Real temp, const Real stoich_dev, Real & cond0, Real & cond0_dt)
{
  Real tnd = temp / 1000.0   ;
  Real tnd2p5 = std::pow(tnd,2.5e0) ;

  // Temperature dependent thermal conductivity, corrected to full density
  const Real A = 2.85*stoich_dev + 0.035;    //(mK/W)
  const Real C = (-0.715*stoich_dev+0.286);          //(m/W)
  const Real d = 1. / (A+C*tnd);
  const Real exp1 = std::exp(-16.35/tnd) ;
  const Real term = d + (6400.0/tnd2p5)*exp1;
  /*const Real term_dT = -C/std::pow(A + C*tnd, 2)
                  - exp1*(5*6400/(2*std::pow(tnd, 7/2)) - 6400*16.35/(std::pow(tnd, 3/2)));*/   // wrong derivative
  const Real term_dT = (-C/std::pow(A + C*tnd, 2) + 6400.0 * exp1 * (16.35 / std::pow(tnd, 9/2) - 2.5 / std::pow(tnd, 7/2))) * 1.0e-3;

  cond0 = 1.158 * term; // (eq 33 Cabajo)
  cond0_dt = 1.158*term_dT;

  //Moose::out << cond0 <<"\n";
}

// model 1 (MOX part)
void
ThermalFuel::computeAmayaUnirradThCond(const Real temp, const Real Pu_content,  Real & cond0, Real & cond0_dt)
{
    if (Pu_content == 0.0)
    {
        std::stringstream msg;
        msg << "ERROR in ThermalFuel: Fink-Amaya requires nonzero Pu content (typically ~ 0.07 for MOX fuel)\n";
        //<< "\t 'model' must be 0 - Duriez model, 1 - Amaya, 2 - Fink-Lucuta, 3 - Halden, 4 - NFIR, or 5 - NFI modified \n";
        mooseError( msg.str() );
    }
    else
    {
        const Real d0(0.209);
        const Real d1(1.09e-3);
        Real g = std::pow(cond0/(d0*std::exp(d1*temp)*Pu_content),0.5);
        Real g_dt = 0.5*(cond0_dt - d1*cond0)/std::pow(d0*std::exp(d1*temp)*Pu_content*cond0, 0.5);
        Real f = std::pow(d0*std::exp(d1*temp)*Pu_content*cond0, 0.5);
        Real f_dt = 0.5*(d1*cond0+cond0_dt)*std::pow(d0*std::exp(d1*temp)*Pu_content/cond0,0.5);
        cond0_dt = g_dt*std::atan(f) + g*f_dt/(1+std::pow(f,2));
        cond0 = g*std::atan(f);
    }
}

// model 3
void
ThermalFuel::computeHaldenUnirradThCond(const Real temp, const Real buuo2, const Real stoich_dev, const Real Gd_content, const Real Pu_content, Real & cond0, Real & cond0_dt)
{
    Real term1 = 1.1599 * Gd_content;
    Real term2 = 1.1599 * stoich_dev;
    Real term3 = 0.004 * buuo2;
    Real term4 = 0.0002475 * (1.0 - 0.00333 * buuo2) * std::min(1650.0, temp - 273.15);
    Real term5 = 0.0132 * std::exp(0.00188 * (temp - 273.15));    // electronic component

    if (Pu_content > 0 )  // for MOX
    {
        Real term7 = 0.92 / (0.1148 + term1 + term2 + term3 + term4) + term5;   // MOX thermal conductivity for 95% TD
        Real term7_dT = -0.92 * (0.0002475 * (1.0 - 0.00333 * buuo2))
            / ((0.1148 + term1 + term2 + term3 + term4) * (0.1148 + term1 + term2 + term3 + term4))
            + 0.0132 * 0.00188 * std::exp(0.00188 * (temp - 273.15));
        cond0 = term7;
        cond0_dt = term7_dT;
    }
    else                    // for UO2
    {
        Real term6 = 1.0 / (0.1148 + term1 + term2 + term3 + term4) + term5;   // UO2 thermal conductivity for 95% TD
        Real term6_dT = -1.0 * (0.0002475 * (1.0 - 0.00333 * buuo2))
            / ((0.1148 + term1 + term2 + term3 + term4) * (0.1148 + term1 + term2 + term3 + term4))
            + 0.0132 * 0.00188 * std::exp(0.00188 * (temp - 273.15));
        cond0 = term6;
        cond0_dt = term6_dT;
    }
}

// model 4
void
ThermalFuel::computeNFIRUnirradThCond(const Real temp, const Real bu, Real porosity, Real & cond0, Real & cond0_dt)
{
  // does not account for Gd or Pu (MOX) effects
  // accounts for burnup, temperature, porosity
  Real tempC = temp - 273.15;
  Real term1 = 6.14e-3 * bu;
  Real term2 = -1.4e-5 * bu * bu;
  Real term3 = -1.81e-6 * bu;
  Real term4 = 2.6e-3 * bu;
  Real term5 = -2.7e-7 * bu;

  Real tanh_term = std::tanh((tempC - 900.0)/150);
  Real thermal_recovery = (1.0 + tanh_term) * 0.5;
  Real dtr_dtC = (1-tanh_term*tanh_term)/300;

  Real p_c_numer = 1.0 - (2.58 - 5.8e-4 * tempC) * porosity;
  Real p_c_denom = 1.0 - 0.05 * (2.58 - 5.8e-4 * tempC);
  Real porosity_correction = p_c_numer / p_c_denom;
  Real dpc_dtC = 5.8e-4*porosity/p_c_denom - 0.05*5.8e-4*p_c_numer/(p_c_denom*p_c_denom);

  Real term6 = 1.32e-2 * std::exp(1.88e-3 * tempC);       // electronic component
  Real dt6_dtC = 1.88e-3 * term6;

  Real kstart = 1.0 / (9.592e-2 + term1 + term2 + (2.5e-4 + term3) * tempC);
  Real dks_dtC = -(2.5e-4+term3)*kstart*kstart;

  Real kend = 1. / (9.592e-2 + term4 + (2.5e-4 + term5) * tempC);
  Real dke_dtC = -(2.5e-4+term5)*kend*kend;

  cond0 = ((1.0 - thermal_recovery) * kstart + thermal_recovery * kend + term6) * porosity_correction;
  cond0_dt = ((1.0 - thermal_recovery) * kstart + thermal_recovery * kend + term6) * dpc_dtC +
    ((1-thermal_recovery)*dks_dtC - dtr_dtC*kstart + thermal_recovery*dke_dtC + dtr_dtC*kend + dt6_dtC)*porosity_correction;
}

// model 5
void
ThermalFuel::computeNFImodUnirradThCond(const Real temp, const Real bu, const Real Gd_content, Real & cond0, Real & cond0_dt)
{
    Real term1 = 1.1599 * Gd_content;
    Real term2 = 2.46e-4 * temp;
    Real term3 = 1.87e-3 * bu;
    Real term4 = (1.0 - 0.9 * std::exp(-0.04 * bu));
    Real term5 = 3.8e-2 * std::pow(bu, 0.28);
    Real term6 = 1.0 + 3.96e2 * std::exp(-6.38e3 / temp);
    Real term7 = 3.5e9 / (temp * temp) * std::exp(-1.636e4 / temp);     // electronic component of k
    cond0 = 1.0 / (0.0452 + term1 + term2 + term3 + term4 * term5 / term6) + term7;
    cond0_dt = 0.0;
}

void
ThermalFuel::computeDissFissProd(const Real temp, const Real bupct, Real & fd, Real & fd_dT)
{
  //Compute correction for dissolved fission products
  fd = 1.0;
  fd_dT = 0.0;

  if ((bupct > 0) && (_model != 3) && (_model != 4) && (_model != 5))
  {
    const Real s(std::sqrt(temp/bupct));
    Real factor = 1.09/std::pow(bupct,3.265) + 0.0643*s;
    Real factor_dT = 0.0643*0.5/s/bupct;
    const Real a(std::atan(1.0/factor));
    fd = factor * a; // eqn 7 (Lucuta)
    fd_dT = factor_dT * a - factor * 1.0/(1.0+1.0/(factor*factor))/(factor*factor)*factor_dT;
  }
}

void
ThermalFuel::computePrecFissProd(const Real temp1, const Real bupct, Real & fp, Real & fp_dT)
{
  fp = 1.0;
  fp_dT = 0.0;

  if ((bupct > 0) && (_model != 3) && (_model != 4) && (_model != 5))
  {
    //Compute correction for precipitated fission products
    Real exp2(exp(-(temp1-1200.)/100.));
    Real factor = 1. + exp2;
    Real factor_dT = -exp2/100.0;
    fp = 1. + ((0.019*bupct)/(3.-0.019*bupct))*(1.0/factor); // eqn 10b (Lucuta)
    fp_dT = -((0.019*bupct)/(3.-0.019*bupct))/(factor*factor)*factor_dT;
  }
}

Real
ThermalFuel::computePor(const Real porosity, const unsigned /*qp*/, const Real /*cond0*/)
{
  //Compute correction for porosity
    switch (_model)
    {
        case 2:         // Fink-Lucuta uses different form of Maxwell-Euken eqn.
            return (1. - porosity) / (1. + 0.5 * porosity);     // eqn 11b (Lucuta)
            break;
        case 3:         //Halden model uses different form of Maxwell-Eucken eqn.
            return (1.0789 * (1 - porosity) / (1 + 0.5 * porosity));    //Halden model uses different form of Maxwell-Eucken eqn.
            break;
        case 4:         // NFIR model has temperature dependent porosity correction built in
            return (1.0);
            break;
        case 5:         //NFI modified model uses different form of Maxwell-Eucken eqn.
            return (1.0789 * (1 - porosity) / (1 + 0.5 * porosity));
        default:        // for Duriez and Fink-Amaya (Lucuta modifications with Carbajo suggestions)
            return (1. - porosity) / (1. + 2*porosity); // eqn 36 (Carbajo)
    }
}

void
ThermalFuel::computeRadDamage(const Real temp1, Real & fr, Real & fr_dT)
{
  // Compute correction for radiation damage
    if ((_model == 3) || (_model == 4) || (_model == 5))
    {
        fr = 1.0;
        fr_dT = 0.0;
    }
    else
    {
        Real exp3(std::exp((temp1-900.)/80.));
        fr = 1. - 0.2/(1. + exp3); // eqn 14f (Lucuta)
        fr_dT = 0.2/((1+exp3)*(1+exp3))*exp3/80;
    }
}

void
ThermalFuel::computeProperties()
{
  for(_qp=0; _qp<_qrule->n_points(); ++_qp)
  {
    Real porosity = _initial_porosity;
    if ( _has_porosity )
      porosity = _porosity[_qp];
    if ( _porosity_material )
      porosity = (*_porosity_mat)[_qp];

    // Normalized temperature raised to power (needed for Fink correlation)
    Real temp1 = _temp[_qp]       ;
    if (temp1 < 0)
    {
      std::stringstream msg;
      msg << "WARNING:  In ThermalFuel:  negative temperature!\n"
          << "\tResetting to zero.\n"
          << "\t_qp: " << _qp << "\n"
          << "\ttemp: " << _temp[_qp] << "\n"
          << "\telem: " << _current_elem->id() << "\n"
          << "\tproc: " << processor_id() << "\n";
      mooseWarning( msg.str() );
      temp1 = 0;
    }

    const Real burnup = _has_burnup ? _burnup[_qp] : _burnup_function->burnup(_q_point[_qp]);

    // Burnup in at.% for Lucuta correction
    Real bupct = burnup * 100.;
    // Burnup in MWd/kgUO2 for Halden correlation
    Real buuo2 = burnup * 950.0 / 1.136;
    // burnup in MWd/kgU for NFIR correlation
    Real bu = burnup * 950.0;

    //Calculate unirradiated thermal conductivity
    Real cond0, cond0_dT;
    switch (_model)
      {
          case 0:           // Duriez (MOX)
              computeDuriezUnirradThCond(temp1, _stoich_dev, cond0, cond0_dT);
              break;
          case 1:           // Fink-Ayama (MOX)
              computeFinkUnirradThCond(temp1, cond0, cond0_dT);
              computeAmayaUnirradThCond(temp1, _Pu_content, cond0, cond0_dT);
              break;
          case 2:           // Fink-Lucuta (UO2)
              computeFinkUnirradThCond(temp1, cond0, cond0_dT);
              break;
          case 3:           // Halden (UO2 (w/wo Gd) or MOX)
              computeHaldenUnirradThCond(temp1, buuo2, _stoich_dev, _Gd_content, _Pu_content, cond0, cond0_dT);
              break;
          case 4:           // NFIR (UO2)
              computeNFIRUnirradThCond(temp1, bu, porosity, cond0, cond0_dT);
              break;
          case 5:           // NFI modified (UO2 (w/wo Gd))
              computeNFImodUnirradThCond(temp1, bu, _Gd_content, cond0, cond0_dT);
              break;
          default:           // Screwup!
              std::stringstream msg;
              msg << "ERROR: In ThermalFuel: unsupported model!\n"
              << "\t 'model' must be 0 - Duriez model or 1 - Amaya or 2 - Fink-Lucuta or 3 - Halden or 4 - NFIR \n";
              mooseError( msg.str() );
      }

    // Correction for dissolved fission products
    Real fd, fd_dT;
    computeDissFissProd(temp1, bupct, fd, fd_dT);

    // Correction for precipitated fission products
    Real fp, fp_dT;
    computePrecFissProd(temp1, bupct, fp, fp_dT);

    // Correction for porosity and fission gas bubbles
    Real fb = computePor(porosity, _qp, cond0);

    // Correction for radiation damage
    Real fr, fr_dT;
    computeRadDamage(temp1, fr, fr_dT);
/*
    switch (_model)
      {
          case 0:       // Duriez
              Moose::err << "Duriez temp1 bupct(at.%) cond0 fd fp fb fr " << temp1 << "  " << bupct << "  " << cond0 << "  " << fd << "  " << fp << "  "
                         << fb << "  " << fr  <<std::endl;
              Moose::err << "Pu_content Gd_content porosity " << _Pu_content << "  " << _Gd_content << "  " << porosity << std::endl;
              break;
          case 1:       // Fink-Amaya
              Moose::err << "Fink-Amaya temp1 bupct(at.%) cond0 fd fp fb fr " << temp1 << "  " << bupct << "  " << cond0 << "  " << fd << "  " << fp << "  "
                         << fb << "  " << fr  <<std::endl;
              Moose::err << "Pu_content Gd_content porosity " << _Pu_content << "  " << _Gd_content << "  " << porosity << std::endl;
              break;
          case 2:       // Fink-Lucuta
              Moose::err << "Fink-Lucuta temp1 bupct(at.%) cond0 fd fp fb fr " << temp1 << "  " << bupct << "  " << cond0 << "  " << fd << "  " << fp << "  "
                         << fb << "  " << fr  <<std::endl;
              Moose::err << "Pu_content Gd_content porosity " << _Pu_content << "  " << _Gd_content << "  " << porosity << std::endl;
              break;
          case 3:       // Halden
              if (_Pu_content > 0.0)
              {
                  Moose::err << "Halden-MOX temp1 buuo2(MWd/kgUO2) cond0 fd fp fb fr " << temp1 << "  " << buuo2 << "  " << cond0 << "  " << fd << "  " << fp << "  "
                             << fb << "  " << fr  <<std::endl;
              }
              else
              {
                  Moose::err << "Halden temp1 buuo2(MWd/kgUO2) cond0 fd fp fb fr " << temp1 << "  " << buuo2 << "  " << cond0 << "  " << fd << "  " << fp << "  "
                             << fb << "  " << fr  <<std::endl;
              }
              Moose::err << "Pu_content Gd_content porosity " << _Pu_content << "  " << _Gd_content << "  " << porosity << std::endl;
              break;
          case 4:       // NFIR
              Moose::err << "NFIR temp1 bu(MWd/kgU) cond0 fd fp fb fr " << temp1 << "  " << bu << "  " << cond0 << "  " << fd << "  " << fp << "  "
                         << fb << "  " << fr  <<std::endl;
              Moose::err << "Pu_content Gd_content porosity " << _Pu_content << "  " << _Gd_content << "  " << porosity << std::endl;
              break;
          case 5:       // NFI modified
              Moose::err << "NFImod temp1 bu(MWd/kgU) cond0 fd fp fb fr " << temp1 << "  " << bu << "  " << cond0 << "  " << fd << "  " << fp << "  "
                         << fb << "  " << fr  <<std::endl;
              Moose::err << "Pu_content Gd_content porosity " << _Pu_content << "  " << _Gd_content << "  " << porosity << std::endl;
              break;
          default:
              Moose::err << "temp1 cond0 fd fp fb fr " << temp1 << "  " << cond0 << "  " << fd << "  " << fp << "  "
                         << fb << "  " << fr  <<std::endl;
              Moose::err << "Pu_content Gd_content porosity " << _Pu_content << "  " << _Gd_content << "  " << porosity << std::endl;
        }
*/

    // Conductivity of irradiated UO2 or MOX: W/(m K)
    _thermal_conductivity[_qp] = cond0*fd*fp*fb*fr;
    //_thermal_conductivity[_qp] = cond0;

    _thermal_conductivity_dT[_qp] =
      cond0_dT * fd*fp*fb*fr +
      fd_dT * cond0*fp*fb*fr +
      fp_dT * cond0*fd*fb*fr +
      fr_dT * cond0*fd*fp*fb;

    // Specific heat: J/(mol K)
    _specific_heat[_qp] = computeSpecificHeat(temp1);

    //   Moose::err << "k rho c " << _thermal_conductivity[_qp] << "  "
    //              << _specific_heat[_qp] <<std::endl;

  }
}
