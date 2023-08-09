#ifndef BIOCRO_H
#define BIOCRO_H

// Commented-out includes are included, directly or indirectly, in the
// previous uncommented-out include.
#include <framework/biocro_simulation.h> // for biocro_simulation
// #include <framework/dynamical_system.h> // for dynamical_system
// #include <framework/state_map.h> // for state_map, state_vector_map
// #include <framework/constants.h> // for math_constants::pi
#include <framework/module_factory.h> // for module_factory
// #include <framework/module_creator.h> // for mc_vector
#include <module_library/module_library.h> // for standardBML::module_library

namespace BioCro {

    using State = state_map;
    using Parameter_set = state_map;
    using Variable_settings = state_map;
    using System_drivers = state_vector_map;
    using Simulation_result = state_vector_map;
    using Module_set = mc_vector;
    using Standard_BioCro_library_module_factory = module_factory<standardBML::module_library>;
    using Module_creator = module_creator*;

    using Simulator = biocro_simulation;
}

// This namespace is simply a kludgy way to prevent direct use of
// global names introduced by the includes above.  (This isn't an
// exhaustive list of introduced names.)
namespace {
    int state_map;
    int state_vector_map;
    int string_vector;
    int mc_vector;
    int module_creator;
    int ode_solver_factory;
    int dynamical_system;
    int biocro_simulation;
    int module;
    int ode_solver;

    template <typename T>
    class module_factory;
}

#endif
