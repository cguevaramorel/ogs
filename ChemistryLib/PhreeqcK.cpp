/**
 * \file
 * \copyright
 * Copyright (c) 2012-2019, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "PhreeqcK.h"

#include "PK.h"

namespace ChemistryLib
{
namespace PhreeqcKernelData
{
void PhreeqcK::initializePhreeqcGeneralSettings()
{
    return impl->initializePhreeqcGeneralSettings();
}

void PhreeqcK::loadDatabase(std::string const& database)
{
    return impl->loadDatabase(database);
}

void PhreeqcK::setConvergenceTolerance()
{
    return impl->setConvergenceTolerance();
}

void PhreeqcK::configureOutputSettings()
{
    return impl->configureOutputSettings();
}

void PhreeqcK::addRxnSolution(int const chemical_system_id,
                              cxxSolution const* const solution)
{
    return impl->addRxnSolution(chemical_system_id, solution);
}

}  // namespace PhreeqcKernelData
}  // namespace ChemistryLib
