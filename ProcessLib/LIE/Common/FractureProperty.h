/**
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include <Eigen/Eigen>

#include "MaterialLib/FractureModels/FracturePermeability.h"
#include "Utils.h"

namespace MeshLib
{
class Element;
}
namespace ProcessLib
{
template <typename T>
struct Parameter;
}
namespace ProcessLib
{
namespace LIE
{
struct FractureProperty
{
    int fracture_id = 0;
    int mat_id = 0;
    Eigen::Vector3d point_on_fracture;
    Eigen::Vector3d normal_vector;
    /// Rotation matrix from global to local coordinates
    Eigen::MatrixXd R;
    /// Initial aperture
    ProcessLib::Parameter<double> const* aperture0 = nullptr;

    virtual ~FractureProperty() = default;
};

struct FracturePropertyHM : public FractureProperty
{
    ProcessLib::Parameter<double> const* specific_storage = nullptr;
    ProcessLib::Parameter<double> const* biot_coefficient = nullptr;

    std::unique_ptr<MaterialLib::Fracture::Permeability> permeability_model;
};

/// configure fracture property based on a fracture element assuming
/// a fracture is a straight line/flat plane
inline void setFractureProperty(unsigned dim, MeshLib::Element const& e,
                                FractureProperty& frac_prop)
{
    auto& n = frac_prop.normal_vector;
    // 1st node is used but using other node is also possible, because
    // a fracture is not curving
    for (int j = 0; j < 3; j++)
        frac_prop.point_on_fracture[j] = e.getNode(0)->getCoords()[j];
    computeNormalVector(e, dim, n);
    frac_prop.R.resize(dim, dim);
    computeRotationMatrix(e, n, dim, frac_prop.R);
    DBUG("Normal vector of the fracture element %d: [%g, %g, %g]", e.getID(),
         n[0], n[1], n[2]);
}

}  // namespace LIE
}  // namespace ProcessLib
