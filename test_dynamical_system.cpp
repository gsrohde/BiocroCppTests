#include <gtest/gtest.h>
#include <gmock/gmock.h> // for matchers

#include <iostream>

#include <framework/dynamical_system.h> // for dynamical_system
// #include <framework/state_map.h> // for state_map, state_vector_map
#include <framework/module_factory.h> // for module_factory
// #include <framework/module_creator.h> // for mc_vector
#include <module_library/module_library.h> // for standardBML::module_library
#include <framework/ode_solver_library/ode_solver_factory.h> // for ode_solver_factory
// #include <framework/ode_solver.h> // for ode_solver

#include "print_result.h"

using namespace std;

// Solver parameters
const string ode_solver_name {"boost_euler"};
constexpr double output_step_size {1};
constexpr double adaptive_rel_error_tol {1e-4};
constexpr double adaptive_abs_error_tol {1e-4};
constexpr int adaptive_max_steps {200};

// Dynamical System parameters
constexpr size_t number_of_timepoints{100};
const string driver_variable_name{"some_driver"};
vector<double> sequence(size_t length) {
    auto v = vector<double>(length);
    iota(v.begin(), v.end(), 0);
    return v;
}

state_map initial_state = { {"position", 0}, {"velocity", 1} };
const state_map invariant_parameters = { {"mass", 10}, {"spring_constant", 0.1}, {"timestep", 1}};
state_vector_map drivers = { {driver_variable_name,  sequence(number_of_timepoints)} };
const mc_vector steady_state_modules(0);
const mc_vector derivative_modules { module_factory<standardBML::module_library>::retrieve("harmonic_oscillator") };

// The solver
const auto system_solver =
    std::unique_ptr<ode_solver>(
        ode_solver_factory::create(
            ode_solver_name,
            output_step_size,
            adaptive_rel_error_tol,
            adaptive_abs_error_tol,
            adaptive_max_steps));

// The system
const dynamical_system ds(initial_state,
                          invariant_parameters,
                          drivers,
                          steady_state_modules,
                          derivative_modules);

// Shared pointer to the system
const auto shared_ds = std::shared_ptr<dynamical_system>(
                     new dynamical_system(
                         initial_state,
                         invariant_parameters,
                         drivers,
                         steady_state_modules,
                         derivative_modules));


// Tests

// get_ntimes() should return the number of time points of the
// simulation as determined by the length of the drivers.
TEST(dynamical_systemTest, get_ntimes) {
    EXPECT_EQ(ds.get_ntimes(), drivers[driver_variable_name].size());
}

// The system we've defined shouldn't require an Euler solver.
TEST(dynamical_systemTest, requires_euler_ode_solver) {
    EXPECT_EQ(ds.requires_euler_ode_solver(), false);
}

// To begin with, the values returned by get_differential_quantities
// should match the values in the initial state.
TEST(dynamical_systemTest, differential_quantities) {
    auto size = initial_state.size();
    auto v = vector<double>(2);
    ds.get_differential_quantities(v);
    string_vector keys{ds.get_differential_quantity_names()};
    for (auto i = 0; i < keys.size(); ++i) {
        EXPECT_DOUBLE_EQ(v[i], initial_state[keys[i]]);
    }
}

using ::testing::HasSubstr;
using ::testing::Not;
using ::testing::MatchesRegex;
TEST(dynamical_systemTest, with_solver) {
    EXPECT_EQ(system_solver->generate_integrate_report(), "The ode_solver has not been called yet");

    auto result = system_solver->integrate(shared_ds);
    print_result(result);

    auto integration_report = system_solver->generate_integrate_report();
    EXPECT_THAT(integration_report, Not(HasSubstr("The ode_solver has not been called yet")));
    EXPECT_THAT(integration_report,
                MatchesRegex("boost::numeric::odeint::integrate_const required " +
                             std::to_string(number_of_timepoints - 1) +
                             " steps to integrate the system\n.*"));
    // (The boost_euler solver does exactly one step for each time
    // interval, hence number_of_timepoints - 1 total.)

    //shared_ds->reset();

    result = system_solver->integrate(shared_ds);
    print_result(result);
}
