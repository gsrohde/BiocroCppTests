_For instructions on using these materials, see [README](README)
(online at [README on
GitHub](https://raw.githubusercontent.com/gsrohde/BioCroCppTests/documentation/README))._

# Towards a C++ Interface for BioCro

For some time now, we have bandied about the idea that the BioCro C++
library should be able to be used on its own without the R interface
provided by the BioCro R package.  The [BioCro
II](https://academic.oup.com/insilicoplants/article/4/1/diac003/6527687)
paper even states "The central framework can conveniently be accessed
through the R package interface or directly through C/C++, and other
interfaces can be developed without duplicating the essential code."
So I have been thinking about what the public interface to the C++
library should look like.  Concomitantly, I have been experimenting
with testing BioCro at the C++ level using the _GoogleTest_ testing
framework.

### Public interface versus publicly-available classes and functions

The number of public functions and methods in the BioCro C++ library
is vast, but the number of R functions that interact with this library
is relatively small—only about ten.  Clearly, a public interface to
the BioCro C++ library need only make available a tiny fraction of the
classes and functions that could be called by a user of the library.

### An experimental Public Interface/Test Suite repository

To test out the design of a public interface, I've created a public
repository `gsrohde/BioCroCppTests` on GitHub.  This consists of a few
public-interface header files, several tests, a _Make_ file for
building the tests, a _Doxyfile_ for configuring builds of the Doxygen
documentation, and a submodule `testBML`, used in testing the use of
user-created module libraries.  Also included is a README file that
tells how to use the repository's collection of files with BioCro, and
finally, `README.md`, the file that you are reading.

## Notes on the interface files

There are two main interface header files, `BioCro.h` and
`BioCro-extended.h`.

`BioCro.h` is intended to provide only the highest-level classes and
data structures needed to use BioCro.so in writing C++ programs.  To
wit, it provides the class most central to BioCro’s functioning,
called here `Simulator`, and it provides the data types needed in
constructing a simulator:

* `State` (for specifying the initial state)
* `Parameter_set`
* `System_drivers`
* `Module_set` (for both the direct and the differential module collections)
* `Standard_BioCro_library_module_factory`

It also provides a declares a name `Simulation_result` for the type
returned by `Simulator.run_simulation`.

I have chosen these names to further distance conceptual types
embodied by these classes from their implementation.  For example,
both`State` and `Parameter_set` are implemented by the standard
library type `unordered_map<string, double>` (aliased to `state_map`
in BioCro).  But I wanted to avoid incorporating hints about their
implementation ("map") into their names; furthermore, they seemed
conceptually distinct enough to give them _different_ names.

As another example, I use the name `Module_set` to emphasize that the
order of the elements used in initializing objects of this class is
immaterial.  (It is also meant to suggest that any given module
appears at most once in a `Module_set`; but this isn’t enforced in the
case of sets of differential modules.)

A drawback using these concept-oriented names is that they give the
user little clue as to how objects of such classes may be used—in
other words, what methods are available when working with such
objects: Most users know roughly what methods are available when
confronted with an object of type `vector<string>`, for example.  But
present them with an object with a type called `Module_names` (which,
as it happens, is aliased to `vector<string>`) and they won't have a
clue of how to work with such an object unless you spell it out.

The solution is to document each of these aliases as if they
were classes defined directly.  I have sketched out a few
Doxygen-style comments to hint at what such documentation might look
like.  But since this is only a rough draft of an interface, I have
stopped short of anything approaching comprehensive user-oriented
documentation.

### Preventing direct use of exposed BioCro classes

To encourage using only the names provided by the interface files, I
use a somewhat kludgy hack involving the use of an anonymous
namespace: Following the _using_ statements in `BioCro.h`, I redefine
the exposed names brought in by the _include_ statements at the top of
`BioCro.h`.  This causes no conflict with the names as originally
defined unless the subsequent code actually _uses_ one of these names.
For example, if we try to use `state_map` in a source file that
includes `BioCro.h`, we will get a compilation error, something like
**"error: reference to 'state_map' is ambiguous"**.

Arguably, the anonymous namespace should redefine _all_ names brought
in by the includes at the top of `BioCro.h`, thus preventing any
direct use of the functions and classes so exposed.  But I didn't try
to make an exhaustive list and instead concentrated on those names
most likely to be used "by mistake."

### Going beyond aliasing

Note that names declared in the interface header files are mostly
aliases.  Thus, the class functions available for classes declared in
these header files are exactly the same as the class functions of the
class aliased.  But we could have taken a further step in distancing
the public interface types from their BioCro implementations: we could
have, for example, made the names into names of classes that wrap
existing class instead of merely aliasing them.  In this way, we could
restrict, if desired, the functions available, or even rename or alter
them in some useful way.

One example where we have done this is the `Single_use_simulator`
class, defined in the file `safe_simulators.h`.  Unlike `Simulator`,
which is merely an alias for BioCro's `biocro_simulation` class,
`Single_use_simulator` presents a modified version of that class in
which `run_simulation` is only allowed to be run once.  (This prevents
possibly erroneous usages of the simulator class.)

A different approach is used by the `Alternative_idempotent_simulator`
class, also defined in `safe_simulators.h`.  Whereas
`Single_use_simulator` inherits from `Simulator`
(a.k.a. `biocro_simulation`) and merely redefines its `run_simulation`
member function to be "safe", `Alternative_idempotent_simulator` uses,
but does not inherit from, the `Simulator` class.  Instead, it creates
a `Simulator` object on the fly whenever its `run_simulation` function
is called, and it delegates to _that_ object's `run_simulation`
function.  This avoids the problem of possibly running
`run_simulation` when the simulator is in a _dirty_ state.

A third approach is taken by `Idempotent_simulator`.  This class
merely copies most of the code in the original `Simulator`
(`biocro_simulation`) class definition but includes a call to the
underlying dynamical system's reset function at the top of the
`run_simulation` function, ensuring the system is in a clean state
before a simulation is run.

## The tests

The focus of the _GoogleTest_ tests is primarily to demonstrate the
use of the proposed interface, both how it should be used and how it
can be misused.  In particular, they demonstrate some of the pitfalls
that await the unwary, pitfalls that R users are largely insulated
from, and possible methods of ameliorating such pitfalls.

It also demonstrates features of _GoogleTest_ itself and various ways
in which it might be used.

Here are a few notes on the individual test files.

* `segfault_test.cpp` (build and run with `make 1`)

   These tests merely demonstrate how _GoogleTest_ can run code that
   causes a segmentation fault without itself crashing, and how it can
   detects segmentation faults.

* `run_test_simulator.cpp` (build and run with `make 10`)

   This file merely demonstrates basic usage of the `Simulator` class.

* `test_dynamical_system.cpp` (build and run with `make 2`)

   These tests test the various class functions available from a
   `BioCro::Dynamical_system` object.

   Many users will be content to use only `BioCro::Simulator` objects.
   `BioCro::Dynamical_system` objects might be considered somewhat
   lower level and thus this name is declared in `BioCro_Extended.h`
   rather than `BioCro.h`.  One possible use of
   `BioCro::Dynamical_system` objects is to be able to easily solver
   one system using a variety of solvers without having to define a
   new Simulation object each time.

* `test_harmonic_oscillator.cpp` (build and run with `make 3`)

   This file tests a Simulator based upon a well-known and studied
   dynamical system, one having an explicit mathematical solution.
   The tests show that the simulation results match the expected
   behavior, given an assortment of randomly-assigned values for the
   input parameters and initial state.

   The tests in this file probably come closest to being true
   regression tests.

* `test_module_creator.cpp` (build and run with `make 4`)

   These tests test the retrieval of `BioCro::Module_creator` objects
   using `BioCro::Standard_BioCro_library_module_factory`'s `retrieve`
   function and the user of a creator's various functions, once such
   an object has been retrieved.

* `test_module_evaluation.cpp` (build and run with `make 5`)

   The tests in the file demonstrate how to use a BioCro module
   directly, outside the context of a simulator or a dynamical system,
   using classes provided by the interface defined in `BioCro.h`.  It
   also demonstrates a pitfall arising from careless usage of the
   interface.

* `test_module_factory_functions.cpp` (build and run with `make 6`)

   This file tests the public member functions of module factory
   objects except for the most important one (`retrieve`), which is
   tested elsewhere.

* `test_module_object.cpp` (build and run with `make 7`)

   Here we demonstrate accessing classes from `BioCro.so` directly,
   not through the `BioCro.h` interface, to construct and run a BioCro
   module directly.  (Compare with `test_module_evaluation`.)  Again,
   it shows the problems that arise when a module is not constructed
   correctly.

* `test_multiple_module_libraries.cpp` (build and run with `make 8`)

   Here, using the standard BioCro module library together with a
   user-defined module library is demonstrated.

* `test_repeat_runs.cpp` (build and run with `make 9`)

   The tests in this file demonstrate a quirk in the Simulator object
   whereby when it is run a second time, the drivers are set back to
   their initial values but the differential quantities are not.  It
   tests out various alternerative versions of a simulator that
   protect against this problem.
