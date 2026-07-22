

# This is a skeleton CMakeLists.txt file, auto-generated on
# Wed Jul 22 16:39:41 UTC 2026.  The developer(s) of this package should delete
# this comment as well as adding dependent targets, packages,
# etc. specific to the package. For details on how to write a package,
# please see
# https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/

cmake_minimum_required(VERSION 3.12)
project(protodqmproc VERSION 0.0.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()


find_package(Boost COMPONENTS unit_test_framework REQUIRED)

##############################################################################


# See https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/#daq_add_library

daq_add_library( LINK_LIBRARIES opmonlib::opmonlib) # We need to link in the opmonlib library for Protobuf-generated code to compile

##############################################################################


# See https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/#daq_add_application

daq_add_application(run_protodqm run_protodqm.cxx LINK_LIBRARIES ) # Any libraries to link in not yet determined

##############################################################################


# See https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/#daq_add_unit_test

daq_add_unit_test(Placeholder_test LINK_LIBRARIES)  # Placeholder_test should be replaced with real unit tests

##############################################################################

daq_install()

