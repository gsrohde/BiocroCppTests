#ifndef BIOCRO_H
#define BIOCRO_H

// Commented-out includes are included, directly or indirectly, in the
// previous uncommented-out include.
#include <framework/biocro_simulation.h> // for biocro_simulation
// #include <framework/state_map.h> // for state_map, state_vector_map
// #include <framework/constants.h> // for math_constants::pi
#include <framework/module_factory.h> // for module_factory
// #include <framework/module_creator.h> // for mc_vector
#include <module_library/module_library.h> // for standardBML::module_library
#include <framework/ode_solver_library/ode_solver_factory.h> // for ode_solver_factory
// #include <framework/ode_solver.h> // for ode_solver

namespace BioCro {

    using State = state_map;
    using Parameter_set = state_map;
    using System_drivers = state_vector_map;
    using Simulation_result = state_vector_map;
    using Module_set = mc_vector;
    using Standard_BioCro_library_module_factory = module_factory<standardBML::module_library>;
    using Module_creator = module_creator*;

    using Simulator = biocro_simulation;

}

#endif
