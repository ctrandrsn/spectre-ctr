// Distributed under the MIT License.
// See LICENSE.txt for details.

#include "Framework/TestingFramework.hpp"

#include "DataStructures/DataVector.hpp"
#include "Evolution/Systems/GrMhd/ValenciaDivClean/Fluxes.hpp"
#include "Framework/CheckWithRandomValues.hpp"
#include "Framework/SetupLocalPythonEnvironment.hpp"

SPECTRE_TEST_CASE("Unit.GrMhd.ValenciaDivClean.Fluxes", "[Unit][GrMhd]") {
  pypp::SetupLocalPythonEnvironment local_python_env{
      "Evolution/Systems/GrMhd/ValenciaDivClean"};

  pypp::check_with_random_values<1>(
      &grmhd::ValenciaDivClean::ComputeFluxes::apply, "Fluxes",
      {"tilde_d_flux", "tilde_ye_flux", "tilde_tau_flux", "tilde_s_flux",
       "tilde_b_flux", "tilde_phi_flux"},
      {{{0.0, 1.0}}}, DataVector{5});
}
