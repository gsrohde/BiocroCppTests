#ifndef VERBOSE
#define VERBOSE false
#endif


#include <gtest/gtest.h>

#include <iostream>

#include "BioCro.h"

#include "Random.h"

using std::cout;
using std::endl;

using Module_factory = BioCro::Standard_BioCro_library_module_factory;

TEST(ModuleEvaluationTest, DifferentialModule) {

    Rand_double double_gen { -100, 100 };
    Rand_double pos_double_gen { 1e-5, 100 };

    BioCro::Module_creator w = Module_factory::retrieve("harmonic_oscillator");
                            
    // input_quantities should be a state map
    // use it to initialize the quantity list
    BioCro::Variable_settings quantities {
        {"position", double_gen()},
        {"velocity", double_gen()},
        // The mass and spring constant must be positive:
        {"mass", pos_double_gen()},
        {"spring_constant", pos_double_gen()}
    };

    BioCro::Variable_settings module_output_map;

    // Get the module's outputs and add them to the output list with default
    // values of 0.0. Since derivative modules add their output values to
    // the values in module_output_map, the result only makes sense if each
    // parameter is initialized to 0.
    auto module_outputs = w->get_outputs();
    for (string param : module_outputs) {
        module_output_map[param] = 0.0;
    }

    auto module_ptr =
        w->create_module(quantities, &module_output_map);

    module_ptr->run();

    if (VERBOSE) {
        for (auto item : quantities) {
            cout << item.first << ": " << item.second << endl;
        }
        for (string param : module_outputs) {
            cout << param << " derivative: "
                 << module_output_map[param] << endl;
        }
    }

    // dx/dt = v    
    EXPECT_DOUBLE_EQ(module_output_map["position"], quantities["velocity"]);
    // dv/dt = a = -kx/m
    EXPECT_DOUBLE_EQ(module_output_map["velocity"],
                     -quantities["spring_constant"] * quantities["position"] / quantities["mass"]);
}

TEST(ModuleEvaluationTest, DirectModule) {

    Rand_double double_gen { -100, 100 };
    Rand_double pos_double_gen { 1e-5, 100 };

    BioCro::Module_creator w = Module_factory::retrieve("solar_position_michalsky");
                            
    // Use values for Urbana, Illinois (40.0932N 88.20175W) at 5:48 CDT on July 19, 2023,
    // the time predicted as the sunrise time on timeanddate.com:
    BioCro::Variable_settings quantities {
        {"lat", 40.0932},
        {"longitude", -88.20175},
        {"time", 200 + 5.0/24 + 48.0/60/24},
        {"time_zone_offset", -5},
        {"year", 2023},
    };

    BioCro::Variable_settings module_output_map;

    // Get the module's outputs and add them to the output list with default
    // values of 0.0. Since derivative modules add their output values to
    // the values in module_output_map, the result only makes sense if each
    // parameter is initialized to 0.
    auto module_outputs = w->get_outputs();
    for (string param : module_outputs) {
        module_output_map[param] = 0.0;
    }

    auto module_ptr =
        w->create_module(quantities, &module_output_map);

    module_ptr->run();

    if (VERBOSE) {
        for (auto item : quantities) {
            cout << item.first << ": " << item.second << endl;
        }
        for (string param : module_outputs) {
            cout << param << ": "
                 << module_output_map[param] << endl;
        }
    }

    EXPECT_NEAR(module_output_map["cosine_zenith_angle"], 0, 1.1e-2);
}
