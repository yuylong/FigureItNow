/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2018 Yulong Yu. All rights reserved.
 */

#include <qmath.h>
#include <QString>

#include "finExecVariable.h"
#include "finExecEnvironment.h"


static finExecVariable *_sysvar_nil();
static finExecVariable *_sysvar_pi();
static finExecVariable *_sysvar_e();

static finExecVariable *_sysvar_kilo();
static finExecVariable *_sysvar_million();
static finExecVariable *_sysvar_billion();
static finExecVariable *_sysvar_trillion();
static finExecVariable *_sysvar_quarter();
static finExecVariable *_sysvar_percent();
static finExecVariable *_sysvar_permille();
static finExecVariable *_sysvar_ppm();
static finExecVariable *_sysvar_ppb();

static finExecVariable *_sysvar_pythagoras_const();
static finExecVariable *_sysvar_theodorus_const();
static finExecVariable *_sysvar_euler_mascheroni_const();
static finExecVariable *_sysvar_golden_ratio();
static finExecVariable *_sysvar_meissel_mertens_const();
static finExecVariable *_sysvar_bernsteins_const();
static finExecVariable *_sysvar_gauss_kuzmin_wirsing_const();
static finExecVariable *_sysvar_hafner_sarnak_mccurley_const();
static finExecVariable *_sysvar_landaus_const();
static finExecVariable *_sysvar_omega_const();
static finExecVariable *_sysvar_golomb_dickman_const();
static finExecVariable *_sysvar_cahens_const();
static finExecVariable *_sysvar_twin_prime_const();
static finExecVariable *_sysvar_laplace_limit();
static finExecVariable *_sysvar_embree_trefethen_const();
static finExecVariable *_sysvar_landau_ramanujan_const();
static finExecVariable *_sysvar_alladi_grinstead_const();
static finExecVariable *_sysvar_prime_quad_bruns_const();
static finExecVariable *_sysvar_catalans_const();
static finExecVariable *_sysvar_legendres_const();
static finExecVariable *_sysvar_lengyels_const();
static finExecVariable *_sysvar_viswanaths_const();
static finExecVariable *_sysvar_aperys_const();
static finExecVariable *_sysvar_conways_const();
static finExecVariable *_sysvar_mills_const();
static finExecVariable *_sysvar_plastic_const();
static finExecVariable *_sysvar_ramanujan_soldner_const();
static finExecVariable *_sysvar_backhouses_const();
static finExecVariable *_sysvar_porters_const();
static finExecVariable *_sysvar_liebs_sqr_ice_const();
static finExecVariable *_sysvar_erdos_borwein_const();
static finExecVariable *_sysvar_nivens_const();
static finExecVariable *_sysvar_twin_primes_bruns_const();
static finExecVariable *_sysvar_universal_parabolic_const();
static finExecVariable *_sysvar_feigenbaum_const_2nd();
static finExecVariable *_sysvar_sierpinskis_const();
static finExecVariable *_sysvar_khinchins_const();
static finExecVariable *_sysvar_fransen_robinson_const();
static finExecVariable *_sysvar_levys_const();
static finExecVariable *_sysvar_reciprocal_fibonacci_const();
static finExecVariable *_sysvar_feigenbaum_const();

static finExecVariable *_sysvar_app_name();
static finExecVariable *_sysvar_app_prime_version();
static finExecVariable *_sysvar_app_version();
static finExecVariable *_sysvar_app_author();

typedef finExecVariable *(*_finSysvarGencall)();

_finSysvarGencall _finSysvarGencallList[] = {
    _sysvar_nil,
    _sysvar_pi,
    _sysvar_e,

    _sysvar_kilo,
    _sysvar_million,
    _sysvar_billion,
    _sysvar_trillion,
    _sysvar_quarter,
    _sysvar_percent,
    _sysvar_permille,
    _sysvar_ppm,
    _sysvar_ppb,

    _sysvar_pythagoras_const,
    _sysvar_theodorus_const,
    _sysvar_euler_mascheroni_const,
    _sysvar_golden_ratio,
    _sysvar_meissel_mertens_const,
    _sysvar_bernsteins_const,
    _sysvar_gauss_kuzmin_wirsing_const,
    _sysvar_hafner_sarnak_mccurley_const,
    _sysvar_landaus_const,
    _sysvar_omega_const,
    _sysvar_golomb_dickman_const,
    _sysvar_cahens_const,
    _sysvar_twin_prime_const,
    _sysvar_laplace_limit,
    _sysvar_embree_trefethen_const,
    _sysvar_landau_ramanujan_const,
    _sysvar_alladi_grinstead_const,
    _sysvar_prime_quad_bruns_const,
    _sysvar_catalans_const,
    _sysvar_legendres_const,
    _sysvar_lengyels_const,
    _sysvar_viswanaths_const,
    _sysvar_aperys_const,
    _sysvar_conways_const,
    _sysvar_mills_const,
    _sysvar_plastic_const,
    _sysvar_ramanujan_soldner_const,
    _sysvar_backhouses_const,
    _sysvar_porters_const,
    _sysvar_liebs_sqr_ice_const,
    _sysvar_erdos_borwein_const,
    _sysvar_nivens_const,
    _sysvar_twin_primes_bruns_const,
    _sysvar_universal_parabolic_const,
    _sysvar_feigenbaum_const_2nd,
    _sysvar_sierpinskis_const,
    _sysvar_khinchins_const,
    _sysvar_fransen_robinson_const,
    _sysvar_levys_const,
    _sysvar_reciprocal_fibonacci_const,
    _sysvar_feigenbaum_const,

    _sysvar_app_name,
    _sysvar_app_author,
    NULL
};

finErrorCode
finExecVariable::installSystemVariables(finExecEnvironment *rootenv)
{
    finErrorCode errcode;
    int success = 0, failed = 0;

    for ( int i = 0; _finSysvarGencallList[i] != NULL; i++ ) {
        finExecVariable *curvar = _finSysvarGencallList[i]();
        if ( curvar == NULL )
            goto item_bad;

        errcode = rootenv->addVariable(curvar);
        if ( finErrorKits::isErrorResult(errcode) )
            goto item_bad;

        success++;
        continue;

item_bad:
        if ( curvar != NULL )
            delete curvar;
        failed++;
    }

    if ( success == 0 && failed == 0 )
        return finErrorKits::EC_REACH_BOTTOM;
    else if ( success == 0 )
        return finErrorKits::EC_NOT_FOUND;
    else if ( failed != 0 )
        return finErrorKits::EC_NORMAL_WARN;
    else
        return finErrorKits::EC_SUCCESS;
}

static finExecVariable *_sysvar_nil()
{
    finErrorCode errcode;
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    errcode = retvar->setName(QString("NIL"));
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setType(finExecVariable::TP_NULL);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setLeftValue();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setWriteProtected();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    return retvar;

err:
    delete retvar;
    return NULL;
}

static inline finExecVariable *
_sysvar_gen_num_var(const QString &name, double val)
{
    finErrorCode errcode;
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    errcode = retvar->setName(name);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setType(finExecVariable::TP_NUMERIC);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setNumericValue(val);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setLeftValue();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setWriteProtected();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    return retvar;

err:
    delete retvar;
    return NULL;
}

static inline finExecVariable *
_sysvar_gen_str_var(const QString &name, const QString &val)
{
    finErrorCode errcode;
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    errcode = retvar->setName(name);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setType(finExecVariable::TP_STRING);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setStringValue(val);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setLeftValue();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setWriteProtected();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    return retvar;

err:
    delete retvar;
    return NULL;
}

static finExecVariable *_sysvar_pi()
{
    return _sysvar_gen_num_var(QString("PI"), M_PI);
}

static finExecVariable *_sysvar_e()
{
    return _sysvar_gen_num_var(QString("E"), M_E);
}

static finExecVariable *_sysvar_kilo()
{
    return _sysvar_gen_num_var(QString("KILO"), 1000.0);
}

static finExecVariable *_sysvar_million()
{
    return _sysvar_gen_num_var(QString("MILLION"), 1.0e+6);
}

static finExecVariable *_sysvar_billion()
{
    return _sysvar_gen_num_var(QString("BILLION"), 1.0e+9);
}

static finExecVariable *_sysvar_trillion()
{
    return _sysvar_gen_num_var(QString("TRILLION"), 1.0e+12);
}

static finExecVariable *_sysvar_quarter()
{
    return _sysvar_gen_num_var(QString("QUARTER"), 0.25);
}

static finExecVariable *_sysvar_percent()
{
    return _sysvar_gen_num_var(QString("PERCENT"), 0.01);
}

static finExecVariable *_sysvar_permille()
{
    return _sysvar_gen_num_var(QString("PERMILLE"), 0.001);
}

static finExecVariable *_sysvar_ppm()
{
    return _sysvar_gen_num_var(QString("PPM"), 1.0e-6);
}

static finExecVariable *_sysvar_ppb()
{
    return _sysvar_gen_num_var(QString("PPB"), 1.0e-9);
}

static finExecVariable *_sysvar_pythagoras_const()
{
    return _sysvar_gen_num_var(QString("PYTHAGORAS_CONST"), sqrt(2.0));
}

static finExecVariable *_sysvar_theodorus_const()
{
    return _sysvar_gen_num_var(QString("THEODORUS_CONST"), sqrt(3.0));
}

static finExecVariable *_sysvar_euler_mascheroni_const()
{
    return _sysvar_gen_num_var(QString("EULER_MASCHERONI_CONST"), 0.57721566490153286060651209008240243);
}

static finExecVariable *_sysvar_golden_ratio()
{
    return _sysvar_gen_num_var(QString("GOLDEN_RATIO"), 1.61803398874989484820458683436563811);
}

static finExecVariable *_sysvar_meissel_mertens_const()
{
    return _sysvar_gen_num_var(QString("MEISSEL_MERTENS_CONST"), 0.26149721284764278375542683860869585);
}

static finExecVariable *_sysvar_bernsteins_const()
{
    return _sysvar_gen_num_var(QString("BERNSTEINS_CONST"), 0.28016949902386913303);
}

static finExecVariable *_sysvar_gauss_kuzmin_wirsing_const()
{
    return _sysvar_gen_num_var(QString("GAUSS_KUZMIN_WIRSING_CONST"), 0.30366300289873265859744812190155623);
}

static finExecVariable *_sysvar_hafner_sarnak_mccurley_const()
{
    return _sysvar_gen_num_var(QString("HAFNER_SARNAK_MCCURLEY_CONST"), 0.35323637185499598454351655043268201);
}

static finExecVariable *_sysvar_landaus_const()
{
    return _sysvar_gen_num_var(QString("LANDAUS_CONST"), 0.5);
}

static finExecVariable *_sysvar_omega_const()
{
    return _sysvar_gen_num_var(QString("OMEGA_CONST"), 0.56714329040978387299996866221035554);
}

static finExecVariable *_sysvar_golomb_dickman_const()
{
    return _sysvar_gen_num_var(QString("GOLOMB_DICKMAN_CONST"), 0.62432998854355087099293638310083724);
}

static finExecVariable *_sysvar_cahens_const()
{
    return _sysvar_gen_num_var(QString("CAHENS_CONST"), 0.6434105462);
}

static finExecVariable *_sysvar_twin_prime_const()
{
    return _sysvar_gen_num_var(QString("TWIN_PRIME_CONST"), 0.66016181584686957392781211001455577);
}

static finExecVariable *_sysvar_laplace_limit()
{
    return _sysvar_gen_num_var(QString("LAPLACE_LIMIT"), 0.66274341934918158097474209710925290);
}

static finExecVariable *_sysvar_embree_trefethen_const()
{
    return _sysvar_gen_num_var(QString("EMBREE_TREFETHEN_CONST"), 0.70258);
}

static finExecVariable *_sysvar_landau_ramanujan_const()
{
    return _sysvar_gen_num_var(QString("LANDAU_RAMANUJAN_CONST"), 0.76422365358922066299069873125009232);
}

static finExecVariable *_sysvar_alladi_grinstead_const()
{
    return _sysvar_gen_num_var(QString("ALLADI_GRINSTEAD_CONST"), 0.8093940205);
}

static finExecVariable *_sysvar_prime_quad_bruns_const()
{
    return _sysvar_gen_num_var(QString("PRIME_QUAD_BRUNS_CONST"), 0.87058838);
}

static finExecVariable *_sysvar_catalans_const()
{
    return _sysvar_gen_num_var(QString("CATALANS_CONST"), 0.91596559417721901505460351493238411);
}

static finExecVariable *_sysvar_legendres_const()
{
    return _sysvar_gen_num_var(QString("LEGENDRES_CONST"), 1.0);
}

static finExecVariable *_sysvar_lengyels_const()
{
    return _sysvar_gen_num_var(QString("LENGYELS_CONST"), 1.0986858055);
}

static finExecVariable *_sysvar_viswanaths_const()
{
    return _sysvar_gen_num_var(QString("VISWANATHS_CONST"), 1.13198824);
}

static finExecVariable *_sysvar_aperys_const()
{
    return _sysvar_gen_num_var(QString("APERYS_CONST"), 1.20205690315959428539973816151144999);
}

static finExecVariable *_sysvar_conways_const()
{
    return _sysvar_gen_num_var(QString("CONWAYS_CONST"), 1.30357726903429639125709911215255189);
}

static finExecVariable *_sysvar_mills_const()
{
    return _sysvar_gen_num_var(QString("MILLS_CONST"), 1.30637788386308069046861449260260571);
}

static finExecVariable *_sysvar_plastic_const()
{
    return _sysvar_gen_num_var(QString("PLASTIC_CONST"), 1.32471795724474602596090885447809734);
}

static finExecVariable *_sysvar_ramanujan_soldner_const()
{
    return _sysvar_gen_num_var(QString("RAMANUJAN_SOLDNER_CONST"), 1.45136923488338105028396848589202744);
}

static finExecVariable *_sysvar_backhouses_const()
{
    return _sysvar_gen_num_var(QString("BACKHOUSES_CONST"), 1.45607494858268967139959535111654356);
}

static finExecVariable *_sysvar_porters_const()
{
    return _sysvar_gen_num_var(QString("PORTERS_CONST"), 1.4670780794);
}

static finExecVariable *_sysvar_liebs_sqr_ice_const()
{
    return _sysvar_gen_num_var(QString("LIEBS_SQR_ICE_CONST"), 1.5396007178);
}

static finExecVariable *_sysvar_erdos_borwein_const()
{
    return _sysvar_gen_num_var(QString("ERDOS_BORWEIN_CONST"), 1.60669515241529176378330152319092458);
}

static finExecVariable *_sysvar_nivens_const()
{
    return _sysvar_gen_num_var(QString("NIVENS_CONST"), 1.70521114010536776428855145343450816);
}

static finExecVariable *_sysvar_twin_primes_bruns_const()
{
    return _sysvar_gen_num_var(QString("TWIN_PRIMES_BRUNS_CONST"), 1.902160583104);
}

static finExecVariable *_sysvar_universal_parabolic_const()
{
    return _sysvar_gen_num_var(QString("UNIVERSAL_PARABOLIC_CONST"), 2.29558714939263807403429804918949039);
}

static finExecVariable *_sysvar_feigenbaum_const_2nd()
{
    return _sysvar_gen_num_var(QString("FEIGENBAUM_CONST_2ND"), 2.50290787509589282228390287321821578);
}

static finExecVariable *_sysvar_sierpinskis_const()
{
    return _sysvar_gen_num_var(QString("SIERPINSKIS_CONST"), 2.58498175957925321706589358738317116);
}

static finExecVariable *_sysvar_khinchins_const()
{
    return _sysvar_gen_num_var(QString("KHINCHINS_CONST"), 2.68545200106530644530971483548179569);
}

static finExecVariable *_sysvar_fransen_robinson_const()
{
    return _sysvar_gen_num_var(QString("FRANSEN_ROBINSON_CONST"), 2.80777024202851936522150118655777293);
}

static finExecVariable *_sysvar_levys_const()
{
    return _sysvar_gen_num_var(QString("LEVYS_CONST"), 3.27582291872181115978768188245384386);
}

static finExecVariable *_sysvar_reciprocal_fibonacci_const()
{
    return _sysvar_gen_num_var(QString("RECIPROCAL_FIBONACCI_CONST"), 3.35988566624317755317201130291892717);
}

static finExecVariable *_sysvar_feigenbaum_const()
{
    return _sysvar_gen_num_var(QString("FEIGENBAUM_CONST"), 4.66920160910299067185320382046620161);
}

static finExecVariable *_sysvar_app_name()
{
    return _sysvar_gen_str_var(QString("APP_NAME"), QString("FigureItNow"));
}

static finExecVariable *_sysvar_app_prime_version()
{
    return _sysvar_gen_str_var(QString("APP_PRIME_VER"), QString("7"));
}

static finExecVariable *_sysvar_app_version()
{
    return _sysvar_gen_str_var(QString("APP_VERSION"), QString("0.3"));
}

static finExecVariable *_sysvar_app_author()
{
    return _sysvar_gen_str_var(QString("APP_AUTHOR"), QString("Yulong Yu"));
}
