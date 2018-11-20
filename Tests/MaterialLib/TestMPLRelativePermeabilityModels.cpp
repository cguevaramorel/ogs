/**
 * \author Norbert Grunwald
 * \date   Oct 22, 2018
 * \brief
 *
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */


#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <array>
#include <iostream>
#include "TestMPL.h"

#include "Tests/TestTools.h"

#include "MaterialLib/MPL/mpMedium.h"
namespace MPL = MaterialPropertyLib;


TEST(MPL, RelativePermeabilityLiakopoulos)
{

    std::stringstream m;
    m << "<medium>\n";
    m << "<phases></phases>\n";
    m << "<properties><property>\n";
    m << "<name>relative_permeability</name>\n";
    m << "<type>RelPermLiakopoulos</type>\n";
    m << "</property></properties>\n";
    m << "</medium>\n";

    auto medium = createTestMaterial(m.str());


    std::vector<double> sL = {
    9.9999999998028E-01,9.9999999989387E-01,9.9999999942893E-01,9.9999999471736E-01,
    9.9999997157361E-01,9.9999984703488E-01,9.9999858501787E-01,9.9999238584557E-01,
    9.9995902750533E-01,9.9962098975068E-01,9.9796050952707E-01,9.9454174716771E-01,
    9.8902530631669E-01,9.8113398271542E-01,9.7062861843405E-01,9.5729640026128E-01,
    9.4094412156303E-01,9.2139388305624E-01,8.9848015302316E-01,8.7204765350013E-01,
    8.4194978107482E-01,8.0804739093289E-01,7.7020783743372E-01,7.2830420163733E-01,
    6.8221465868656E-01,6.3182195215141E-01,5.7701295174265E-01,5.1767827708163E-01,
    4.5371197456562E-01,3.8501123745615E-01,3.1147616155240E-01,2.3300953045914E-01,
    2.0000000000000E-01};

    // expected-vector contains:
    // k_rel_L, dkrel_Ldpc, k_rel_G, dkrel_Gdpc
    std::vector<std::array<double, 4> > expected =
    {
    {{9.99999999967701E-01, 1.65765494126300E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99999999822600E-01, 1.69175862575304E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99999999025801E-01, 1.72656009393319E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99999990742369E-01, 1.77366769214155E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99999949159000E-01, 1.81015511034861E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99999720791707E-01, 1.84739313828103E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99997346754875E-01, 1.89779738804693E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99985428923563E-01, 1.93683837084730E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99919978645424E-01, 1.97668249439588E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.99239577490317E-01, 2.03061427344026E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.95823915993033E-01, 2.07238752990070E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.88689694619763E-01, 2.09722056252473E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.77065805639654E-01, 2.11502013467633E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.60315739491178E-01, 2.12893052386876E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.37886129342007E-01, 2.14036402583915E+00, 1.00000000000000E-04, 0.00000000000000E+00}},
    {{9.09281588033024E-01, 2.15007880631957E+00, 2.48958913998505E-04, -1.73828730311531E-02}},
    {{8.74050122056546E-01, 2.15852976605236E+00, 6.53816431636937E-04, -3.29293317913430E-02}},
    {{8.31773775546837E-01, 2.16601160811521E+00, 1.52868961302851E-03, -5.76684797535977E-02}},
    {{7.82062204199259E-01, 2.17272631606950E+00, 3.25970606176880E-03, -9.48652537901063E-02}},
    {{7.24548036937553E-01, 2.17881843859569E+00, 6.44866460643363E-03, -1.48245631344356E-01}},
    {{6.58883398952379E-01, 2.18439502499982E+00, 1.19855290974428E-02, -2.21884476072518E-01}},
    {{5.84737226419692E-01, 2.18953759046687E+00, 2.11280362743922E-02, -3.20030295958856E-01}},
    {{5.01793141937527E-01, 2.19430965535906E+00, 3.55828532985993E-02, -4.46850874551125E-01}},
    {{4.09747739678316E-01, 2.19876168962374E+00, 5.75795140665503E-02, -6.06077557203305E-01}},
    {{3.08309177784130E-01, 2.20293446598656E+00, 8.99239530637100E-02, -8.00515614176668E-01}},
    {{1.97196006267170E-01, 2.20686139981691E+00, 1.36012166575960E-01, -1.03136952269221E+00}},
    {{7.61361788494180E-02, 2.21057022251332E+00, 1.99775044581549E-01, -1.29729583346776E+00}},
    {{0.00000000000000E+00, 0.00000000000000E+00, 2.85509871041826E-01, -1.59301811171718E+00}},
    {{0.00000000000000E+00, 0.00000000000000E+00, 3.97525290330626E-01, -1.90714449236491E+00}},
    {{0.00000000000000E+00, 0.00000000000000E+00, 5.39463579002663E-01, -2.21824334967910E+00}},
    {{0.00000000000000E+00, 0.00000000000000E+00, 7.12984239387685E-01, -2.48582562833627E+00}},
    {{0.00000000000000E+00, 0.00000000000000E+00, 9.14649928858579E-01, -2.61369672245684E+00}},
    {{0.00000000000000E+00, 0.00000000000000E+00, 1.00000000000000E+00, -2.50000000000000E+00}}
    };

    enum {liquid, gas};
    enum {kRel_L, dkRel_LdsL, kRel_G, dkRel_GdsL};

    for (size_t i=0; i<sL.size(); i++)
    {
        MPL::VariableArray variables;
        variables[MPL::Variables::liquid_saturation] = sL[i];

        auto const k_rel = MPL::getPair(medium.property(
                MPL::PropertyEnum::relative_permeability), variables);
        auto const dk_rel_dpc = MPL::getPairDerivative(medium.property(
                MPL::PropertyEnum::relative_permeability), variables,
                MPL::Variables::liquid_saturation);

//        std::cout << std::setprecision(16);
//        std::cout << sL[i] << " " << k_rel[0] << " " << dk_rel_dpc[0] <<
//                " " << k_rel[gas] << " " << dk_rel_dpc[1] << "\n";



        ASSERT_NEAR (k_rel[liquid], expected[i][kRel_L], 1e-12);
        ASSERT_NEAR (dk_rel_dpc[liquid], expected[i][dkRel_LdsL], 1e-12);

        ASSERT_NEAR (k_rel[gas], expected[i][kRel_G], 1e-12);
        ASSERT_NEAR (dk_rel_dpc[gas], expected[i][dkRel_GdsL], 1e-12);

    }
}

TEST(MPL, RelativePermeabilityBrooksCorey)
{

    std::stringstream m;
    m << "<medium>\n";
    m << "<phases></phases>\n";
    m << "<properties><property>\n";
    m << "<name>relative_permeability</name>\n";
    m << "<type>RelPermBrooksCorey</type>\n";
    m << "<residual_liquid_saturation> 0.2 </residual_liquid_saturation>\n";
    m << "<residual_gas_saturation> 0.3 </residual_gas_saturation>\n";
    m << "<lambda> 2.0 </lambda>\n";
    m << "</property></properties>\n";
    m << "</medium>\n";

    auto medium = createTestMaterial(m.str());


    std::vector<double> sL = {
    0.20,0.22,0.24,0.26,0.28,0.30,0.32,0.34,0.36,0.38,0.40,0.42,0.44,
    0.46,0.48,0.50,0.52,0.54,0.56,0.58,0.60,0.62,0.64,0.66,0.68,0.70};

    // expected-vector contains:
    // k_rel_L, dkrel_Ldpc, k_rel_G, dkrel_Gdpc
    std::vector<std::array<double, 4> > expected =
    {
            {{0.0000000E+00, 0.0000000E+00, 1.0000000E+00, -4.0000000E+00}},
            {{2.5600000E-06, 5.1200000E-04, 9.2012544E-01, -3.9813120E+00}},
            {{4.0960000E-05, 4.0960000E-03, 8.4098304E-01, -3.9272960E+00}},
            {{2.0736000E-04, 1.3824000E-02, 7.6324864E-01, -3.8410240E+00}},
            {{6.5536000E-04, 3.2768000E-02, 6.8753664E-01, -3.7255680E+00}},
            {{1.6000000E-03, 6.4000000E-02, 6.1440000E-01, -3.5840000E+00}},
            {{3.3177600E-03, 1.1059200E-01, 5.4433024E-01, -3.4193920E+00}},
            {{6.1465600E-03, 1.7561600E-01, 4.7775744E-01, -3.2348160E+00}},
            {{1.0485760E-02, 2.6214400E-01, 4.1505024E-01, -3.0333440E+00}},
            {{1.6796160E-02, 3.7324800E-01, 3.5651584E-01, -2.8180480E+00}},
            {{2.5600000E-02, 5.1200000E-01, 3.0240000E-01, -2.5920000E+00}},
            {{3.7480960E-02, 6.8147200E-01, 2.5288704E-01, -2.3582720E+00}},
            {{5.3084160E-02, 8.8473600E-01, 2.0809984E-01, -2.1199360E+00}},
            {{7.3116160E-02, 1.1248640E+00, 1.6809984E-01, -1.8800640E+00}},
            {{9.8344960E-02, 1.4049280E+00, 1.3288704E-01, -1.6417280E+00}},
            {{1.2960000E-01, 1.7280000E+00, 1.0240000E-01, -1.4080000E+00}},
            {{1.6777216E-01, 2.0971520E+00, 7.6515840E-02, -1.1819520E+00}},
            {{2.1381376E-01, 2.5154560E+00, 5.5050240E-02, -9.6665600E-01}},
            {{2.6873856E-01, 2.9859840E+00, 3.7757440E-02, -7.6518400E-01}},
            {{3.3362176E-01, 3.5118080E+00, 2.4330240E-02, -5.8060800E-01}},
            {{4.0960000E-01, 4.0960000E+00, 1.4400000E-02, -4.1600000E-01}},
            {{4.9787136E-01, 4.7416320E+00, 7.5366400E-03, -2.7443200E-01}},
            {{5.9969536E-01, 5.4517760E+00, 3.2486400E-03, -1.5897600E-01}},
            {{7.1639296E-01, 6.2295040E+00, 9.8304000E-04, -7.2704000E-02}},
            {{8.4934656E-01, 7.0778880E+00, 1.2544000E-04, -1.8688000E-02}},
            {{1.0000000E+00, 8.0000000E+00, 0.0000000E+00, 0.0000000E+00}}
    };

    enum {liquid, gas};
    enum {kRel_L, dkRel_LdsL, kRel_G, dkRel_GdsL};

    for (size_t i=0; i<sL.size(); i++)
    {
        MPL::VariableArray variables;
        variables[MPL::Variables::liquid_saturation] = sL[i];

        auto const k_rel = MPL::getPair(medium.property(
                MPL::PropertyEnum::relative_permeability), variables);
        auto const dk_rel_dpc = MPL::getPairDerivative(medium.property(
                MPL::PropertyEnum::relative_permeability), variables,
                MPL::Variables::liquid_saturation);

        ASSERT_NEAR (k_rel[liquid], expected[i][kRel_L], 1e-12);
        ASSERT_NEAR (dk_rel_dpc[liquid], expected[i][dkRel_LdsL], 1e-12);

        ASSERT_NEAR (k_rel[gas], expected[i][kRel_G], 1e-12);
        ASSERT_NEAR (dk_rel_dpc[gas], expected[i][dkRel_GdsL], 1e-12);

    }
}
