/**
 * \author Norbert Grunwald
 * \date   Sep 21, 2017
 * \brief
 *
 * \copyright
 * Copyright (c) 2012-2017, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */
#pragma once

#include "MaterialLib/MPL/mpProperty.h"

namespace MaterialPropertyLib
{
class Medium;
class Phase;
class Component;

/**
 *
 *  \brief Viscosity correlation for carbon dioxide, valid in the
 *  temperature range 200K <= T <= 1500 K and for densities up to
 *  1400 kg per cubic metre.
 *  \cite Fenghour:1998
 */
class ViscosityCO2Fenghour final : public Property
{
private:
    Component* _component;

public:
    ViscosityCO2Fenghour(Medium* /*unused*/);
    ViscosityCO2Fenghour(Phase* /*unused*/);
    ViscosityCO2Fenghour(Component* /*c*/);

    /// This method overrides the base class implementation and
    /// actually computes and sets the property _value.
    PropertyDataType value(VariableArray const& /*unused*/) override;
};

}  // namespace MaterialPropertyLib