// Distributed under the MIT License.
// See LICENSE.txt for details.

#pragma once

#include <array>
#include <cstddef>
#include <limits>

#include "DataStructures/DataVector.hpp"
#include "DataStructures/Tensor/Tensor.hpp"
#include "DataStructures/Tensor/TypeAliases.hpp"
#include "Options/Context.hpp"
#include "Options/String.hpp"
#include "PointwiseFunctions/AnalyticData/AnalyticData.hpp"
#include "PointwiseFunctions/Hydro/EquationsOfState/EquationOfState.hpp"
#include "PointwiseFunctions/Hydro/EquationsOfState/IdealFluid.hpp"
#include "PointwiseFunctions/Hydro/Tags.hpp"
#include "PointwiseFunctions/InitialDataUtilities/InitialData.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

/// \cond
namespace PUP {
class er;
}  // namespace PUP
/// \endcond

namespace NewtonianEuler::AnalyticData {
/*!
 * \brief A cylindrical or spherical Sod explosion \cite Toro2009 \cite Sod19781
 *
 * Common initial conditions are:
 *
 * \f{align*}{
 * (\rho, v^i, p) =
 * &\left\{
 * \begin{array}{ll}
 *   (1 ,0, 1) & \mathrm{if} \; r \le 0.5 \\
 *   (0.125 ,0, 0.1) & \mathrm{if} \; r > 0.5
 * \end{array}\right.
 * \f}
 *
 * where \f$r\f$ is the cylindrical (2d) or spherical (3d) radius. This test
 * problem uses an adiabatic index of 1.4. A reference solution can be computed
 * in 1d by solving the Newtonian Euler equations in cylindrical or spherical
 * symmetry. Note that the inner and outer density and pressure, as well as
 * where the initial discontinuity is can be chosen arbitrarily.
 */
template <size_t Dim>
class SodExplosion : public evolution::initial_data::InitialData,
                     public MarkAsAnalyticData {
 public:
  static_assert(Dim > 1, "Sod explosion is a 2d and 3d problem.");
  using equation_of_state_type = EquationsOfState::IdealFluid<false>;

  /// Initial radius of the discontinuity
  struct InitialRadius {
    using type = double;
    static constexpr Options::String help = {
        "The initial radius of the discontinuity."};
    static type lower_bound() { return 0.0; }
  };

  struct InnerMassDensity {
    using type = double;
    static constexpr Options::String help = {"The inner mass density."};
    static type lower_bound() { return 0.0; }
  };

  struct InnerPressure {
    using type = double;
    static constexpr Options::String help = {"The inner pressure."};
    static type lower_bound() { return 0.0; }
  };

  struct OuterMassDensity {
    using type = double;
    static constexpr Options::String help = {"The outer mass density."};
    static type lower_bound() { return 0.0; }
  };

  struct OuterPressure {
    using type = double;
    static constexpr Options::String help = {"The outer pressure."};
    static type lower_bound() { return 0.0; }
  };

  using options = tmpl::list<InitialRadius, InnerMassDensity, InnerPressure,
                             OuterMassDensity, OuterPressure>;

  static constexpr Options::String help = {
      "Cylindrical or spherical Sod explosion."};

  SodExplosion() = default;
  SodExplosion(const SodExplosion& /*rhs*/) = default;
  SodExplosion& operator=(const SodExplosion& /*rhs*/) = default;
  SodExplosion(SodExplosion&& /*rhs*/) = default;
  SodExplosion& operator=(SodExplosion&& /*rhs*/) = default;
  ~SodExplosion() override = default;

  auto get_clone() const
      -> std::unique_ptr<evolution::initial_data::InitialData> override;

  /// \cond
  explicit SodExplosion(CkMigrateMessage* msg);
  using PUP::able::register_constructor;
  WRAPPED_PUPable_decl_template(SodExplosion);
  /// \endcond

  SodExplosion(double initial_radius, double inner_mass_density,
               double inner_pressure, double outer_mass_density,
               double outer_pressure, const Options::Context& context = {});

  /// Retrieve a collection of hydrodynamic variables at position x
  template <typename... Tags>
  tuples::TaggedTuple<Tags...> variables(
      const tnsr::I<DataVector, Dim, Frame::Inertial>& x,
      tmpl::list<Tags...> /*meta*/) const {
    return {tuples::get<Tags>(variables(x, tmpl::list<Tags>{}))...};
  }

  const equation_of_state_type& equation_of_state() const {
    return equation_of_state_;
  }

  // NOLINTNEXTLINE(google-runtime-references)
  void pup(PUP::er& /*p*/) override;

 private:
  tuples::TaggedTuple<hydro::Tags::RestMassDensity<DataVector>> variables(
      const tnsr::I<DataVector, Dim, Frame::Inertial>& x,
      tmpl::list<hydro::Tags::RestMassDensity<DataVector>> /*meta*/) const;

  tuples::TaggedTuple<
      hydro::Tags::SpatialVelocity<DataVector, Dim, Frame::Inertial>>
  variables(const tnsr::I<DataVector, Dim, Frame::Inertial>& x,
            tmpl::list<hydro::Tags::SpatialVelocity<
                DataVector, Dim, Frame::Inertial>> /*meta*/) const;

  tuples::TaggedTuple<hydro::Tags::Pressure<DataVector>> variables(
      const tnsr::I<DataVector, Dim, Frame::Inertial>& x,
      tmpl::list<hydro::Tags::Pressure<DataVector>> /*meta*/) const;

  tuples::TaggedTuple<hydro::Tags::SpecificInternalEnergy<DataVector>>
  variables(
      const tnsr::I<DataVector, Dim, Frame::Inertial>& x,
      tmpl::list<hydro::Tags::SpecificInternalEnergy<DataVector>> /*meta*/)
      const;

  template <size_t SpatialDim>
  friend bool
  operator==(  // NOLINT (clang-tidy: readability-redundant-declaration)
      const SodExplosion<SpatialDim>& lhs, const SodExplosion<SpatialDim>& rhs);

  double initial_radius_ = std::numeric_limits<double>::signaling_NaN();
  double inner_mass_density_ = std::numeric_limits<double>::signaling_NaN();
  double inner_pressure_ = std::numeric_limits<double>::signaling_NaN();
  double outer_mass_density_ = std::numeric_limits<double>::signaling_NaN();
  double outer_pressure_ = std::numeric_limits<double>::signaling_NaN();
  equation_of_state_type equation_of_state_{};
};

template <size_t Dim>
bool operator!=(const SodExplosion<Dim>& lhs, const SodExplosion<Dim>& rhs);
}  // namespace NewtonianEuler::AnalyticData
