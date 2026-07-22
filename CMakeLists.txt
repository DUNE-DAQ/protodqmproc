cmake_minimum_required(VERSION 3.12)
project(protodqmproc VERSION 0.0.0)

find_package(daq-cmake REQUIRED)
find_package(CLI11 REQUIRED)
find_package(fmt REQUIRED)

daq_setup_environment()
find_package(hdf5libs REQUIRED)
find_package(trgdataformats REQUIRED)
find_package(fddetdataformats REQUIRED)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

##############################################################################


# See https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/#daq_add_library

daq_add_library(*.cpp LINK_LIBRARIES CLI11::CLI11 hdf5libs::hdf5libs fddetdataformats::fddetdataformats)

##############################################################################


# See https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/#daq_add_application

daq_add_application(protodqm_run_dqm run_dqm.cxx LINK_LIBRARIES ${PROJECT_NAME}) # Any libraries to link in not yet determined

##############################################################################


# See https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/#daq_add_unit_test

#daq_add_unit_test(Placeholder_test LINK_LIBRARIES)  # should be replaced with real unit tests

##############################################################################

daq_install()

