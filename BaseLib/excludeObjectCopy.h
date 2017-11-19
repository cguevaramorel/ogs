/**
 *
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include <vector>
#include <algorithm>

namespace BaseLib
{

/// excludeObjectCopy copies only those objects that position within the source
/// vector is not in the exclude_positions vector. The implementation of the
/// algorithm requires that the given positions in exclude_positions are sorted
/// in ascending order.
/// @param src_vec the vector of source objects
/// @param exclude_positions the positions of objects in the source vector that
/// do not have to be copied
/// @return vector that contains the copied objects
template <typename Container>
Container excludeObjectCopy(Container const& src_vec,
                            std::vector<std::size_t> const& exclude_positions)
{
    Container dest_vec;
        dest_vec = src_vec;
        return dest_vec;
    }

    assert (exclude_positions.back() < src_vec.size());

    std::copy_n(src_vec.cbegin(), exclude_positions[0], std::back_inserter(dest_vec));
    for (std::size_t i=1; i<exclude_positions.size(); ++i) {
        std::copy_n(
            src_vec.cbegin()+exclude_positions[i-1]+1,
            exclude_positions[i]-(exclude_positions[i-1]+1),
            std::back_inserter(dest_vec)
        );
    }
    std::copy(src_vec.cbegin()+exclude_positions.back()+1,
        src_vec.cend(), std::back_inserter(dest_vec));

    return dest_vec;
}

template <typename Container>
void excludeObjectCopy(Container const& src_vec,
                       std::vector<std::size_t> const& exclude_positions,
                       Container& dest_vec)
{
    dest_vec = excludeObjectCopy(src_vec, exclude_positions);
}


} // end namespace BaseLib
