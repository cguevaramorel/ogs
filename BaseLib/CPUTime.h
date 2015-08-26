/**
 * \file
 * \author Thomas Fischer
 * \author Wenqing Wang
 * \date   2012-05-10, 2014.10.10
 * \brief  Definition of the CPUTime class.
 *
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include <ctime>

namespace BaseLib
{

/// Count CPU time
class CPUTime
{
    public:
        /// Creates the timer and starts it.
        CPUTime() : _start_time(clock())
        {
        }

        /// Start the timer.
        void start()
        {
            _start_time = clock();
        }

        /// Get the elapsed time after started.
        double elapsed() const
        {
            return (clock() - _start_time)/static_cast<double>(CLOCKS_PER_SEC);
        }

        /// Resets the clock and returns the elapsed time so far.
        double restart()
        {
            double const t = elapsed();
            start();
            return t;
        }

    private:
        double _start_time = 0.;
};

} // end namespace BaseLib
