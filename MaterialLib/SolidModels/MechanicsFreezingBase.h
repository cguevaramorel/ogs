/**
 * \copyright
 * Copyright (c) 2012-2016, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#ifndef MATERIALLIB_SOLIDMODELS_MECHANICSFREEZINGBASE_H_
#define MATERIALLIB_SOLIDMODELS_MECHANICSFREEZINGBASE_H_

#include <memory>

#include "ProcessLib/Deformation/BMatrixPolicy.h"
#include "ProcessLib/Parameter/Parameter.h"

namespace MeshLib
{
class Element;
}

namespace MaterialLib
{
namespace Solids
{
/// Interface for mechanical solid material models. Provides updates of the
/// stress for a given current state and also a tangent at that position. If the
/// implemented material model stores an internal state, the nested
/// MaterialStateVariables class should be used; it's only responsibility is to
/// provide state's push back possibility.
template <int DisplacementDim>
struct MechanicsFreezingBase
{
    /// The MaterialStateVariables may store material model specific state
    /// (other than sigma and eps), which are usually material history
    /// dependent. The objects are stored by the user (usually in assembly per
    /// integration point) and are created via \ref
    /// createMaterialStateVariables().
    struct MaterialStateVariables
    {
        virtual ~MaterialStateVariables() = default;
        virtual void pushBackState() = 0;
    };

    /// Polymorphic creator for MaterialStateVariables objects specific for a
    /// material model.
    virtual std::unique_ptr<MaterialStateVariables>
    createMaterialStateVariables() = 0;

    using KelvinVector = ProcessLib::KelvinVectorType<DisplacementDim>;
    using KelvinMatrix = ProcessLib::KelvinMatrixType<DisplacementDim>;

    /// Dynamic size Kelvin vector and matrix wrapper for the polymorphic
    /// constitutive relation compute function.
    /// Returns false in case of errors in the computation if Newton iterations
    /// did not converge, for example.
    bool computeFreezingConstitutiveRelation(
        double const t,
        ProcessLib::SpatialPosition const& x,
        double const dt,
        Eigen::Matrix<double, Eigen::Dynamic, 1> const& eps_prev,
        Eigen::Matrix<double, Eigen::Dynamic, 1> const& eps,
        Eigen::Matrix<double, Eigen::Dynamic, 1> const& sigma_prev,
        Eigen::Matrix<double, Eigen::Dynamic, 1>& sigma,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>&
            C_ice,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>&
            C_solid,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>&
            C,
        double phi_i,
        MaterialStateVariables& material_state_variables)
    {
        // TODO Avoid copies of data:
        // Using MatrixBase<Derived> not possible because template functions
        // cannot be virtual. Maybe there is a workaround for this.  Using
        // Map<Matrix<double, ...>> makes the interface (for the material model
        // implementation) unnecessary difficult.
        KelvinVector const eps_prev_{eps_prev};
        KelvinVector const eps_{eps};
        KelvinVector const sigma_prev_{sigma_prev};
        KelvinVector sigma_{sigma};
        KelvinMatrix C_{C};
        KelvinMatrix C_ice_{C_ice};
        KelvinMatrix C_solid_{C_solid};
        bool const result =
            computeFreezingConstitutiveRelation(t,
                                        x,
                                        dt,
                                        eps_prev_,
                                        eps_,
                                        sigma_prev_,
                                        sigma_,
                                        C_,
                                        C_ice_,
                                        C_solid_,
                                        phi_i,
                                        material_state_variables);

        sigma = sigma_;
        //C_ice = C_ice_, //?
        //C_solid = C_solid_, // ? necessary
        C = C_;
        return result;
    }

    /// Computation of the constitutive relation for specific material model.
    /// This should be implemented in the derived model. Fixed Kelvin vector and
    /// matrix size version; for dynamic size arguments there is an overloaded
    /// wrapper function.
    /// Returns false in case of errors in the computation if Newton iterations
    /// did not converge, for example.
    virtual bool computeFreezingConstitutiveRelation(
        double const t,
        ProcessLib::SpatialPosition const& x,
        double const dt,
        KelvinVector const& eps_prev,
        KelvinVector const& eps,
        KelvinVector const& sigma_prev,
        KelvinVector& sigma,
        KelvinMatrix& C_solid,
        KelvinMatrix& C_ice,
        KelvinMatrix& C,
        double phi_i,
        MaterialStateVariables& material_state_variables) = 0;

    virtual ~MechanicsFreezingBase() = default;
};

}  // namespace Solids
}  // namespace MaterialLib

#endif  // MATERIALLIB_SOLIDMODELS_MECHANICSBASE_H_
