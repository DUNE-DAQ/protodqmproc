/**
 * @file run_dqm.cxx
 *
 * An executable to exercise prototype DQM components/functionality
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
#include <filesystem>

#include "hdf5libs/HDF5RawDataFile.hpp"
#include "daqdataformats/Fragment.hpp"
#include "fddetdataformats/WIBEthFrame.hpp"

using namespace dunedaq;


 /**
  * @brief A struct with available CLI application options
  */
struct
CLIOptions
{
  /// @brief input filename
  std::filesystem::path input_file;
};

/**
 * @brief Parses input arguments and passes them to our CLI app
 * 
 * @param _app CLI application
 * @param _opts Struct with the available options
 */
void
parse_app(CLI::App& _app, CLIOptions& _opts)
{
  _app.add_option("input_file", _opts.input_file, "Input filename")
    ->required()
    ->check(CLI::ExistingFile);
}

/**
 * @brief Gets the hdf5 file objects
 * 
 * @param _filename string with the filename
 * @return std::shared_ptr<hdf5libs::HDF5RawDataFile> pointer to the hdf5 object
 */
std::shared_ptr<hdf5libs::HDF5RawDataFile>
get_file(const std::string& _filename)
{
  std::shared_ptr<hdf5libs::HDF5RawDataFile> file_hdf5;

  file_hdf5 = std::make_shared<hdf5libs::HDF5RawDataFile>(_filename);
  if (!file_hdf5->is_trigger_record_type()) {
    throw std::runtime_error(fmt::format("ERROR: input file '{}' not of type 'TriggerRecord'", _filename));
  }

  std::string application_name = file_hdf5->get_attribute<std::string>("application_name");
  std::cout << "File '" << _filename << "' has application name: " << application_name << std::endl;

  return file_hdf5;
}

std::pair<uint64_t, uint64_t>
get_available_slice_id_range(const std::shared_ptr<hdf5libs::HDF5RawDataFile>& _file)
{
  if (!_file) {
    throw std::runtime_error("No valid HDF5 object provided");
  }

  // Find min/max record id for this application
  auto record_ids = _file->get_all_record_ids();
  if (record_ids.empty()) {
    throw std::runtime_error("HDF5 object contains no records.");
  }
  uint64_t start = record_ids.begin()->first;
  uint64_t end = record_ids.rbegin()->first;

  std::cout << "RecordID range: [" << start << "," << end << "]" << std::endl;

  return {start, end};
}

uint64_t
get_wibeth_count(const std::shared_ptr<hdf5libs::HDF5RawDataFile>& _file_hdf5,
                 const std::string& _frag_dataset)
{
  // Get the fragment and print its type
  std::unique_ptr<daqdataformats::Fragment> frag_ptr = _file_hdf5->get_frag_ptr(_frag_dataset);
  dunedaq::daqdataformats::FragmentType frag_type = frag_ptr->get_fragment_type();
  std::cout << "Fragment type: " << daqdataformats::fragment_type_to_string(frag_type) << std::endl;

  // Only process WIBEth frames
  if (frag_type != daqdataformats::FragmentType::kWIBEth) {
    std::cout << "Fragment is not of type WIBEthFrame, skipping.\n";
    return 0;
  }

  // Get the number of WIBEth frames in the fragment
  int num_frames = (frag_ptr->get_size() - sizeof(daqdataformats::FragmentHeader)) / sizeof(dunedaq::fddetdataformats::WIBEthFrame);
  std::cout << "Number of WIBEthFrames in fragment: " << num_frames << std::endl;
  return num_frames;
}

int
main(int argc, const char* argv[])
{
  CLI::App app{"A prototype DQM application that reads a file and exercises DQM components"};
  CLIOptions opts{};

  parse_app(app, opts);

  try {
    app.parse(argc, argv);
  }
  catch (const CLI::ParseError& error) {
    return app.exit(error);
  }

  std::cout << "Processing file:" << opts.input_file << "\n";

  // Gets the file
  std::shared_ptr<hdf5libs::HDF5RawDataFile> file_hdf5 = 
    get_file(opts.input_file);

  // Get the available record_id range
  std::pair<uint64_t, uint64_t> recordid_range = get_available_slice_id_range(file_hdf5);
  uint64_t recordid_to_process = recordid_range.first;

  uint64_t num_wibeth_frames_count = 0;
  uint64_t num_wibeth_count = 0;

  for (const std::string& frag_dataset : file_hdf5->get_fragment_dataset_paths(recordid_to_process)) {
    std::cout << "Fragment dataset path: " << frag_dataset << "\n";

    // I love this new C++ feature
    if (uint64_t num_wibs = get_wibeth_count(file_hdf5, frag_dataset)){
      num_wibeth_count += num_wibs;
      num_wibeth_frames_count++;
    }
  }

  std::cout << "Total number of WIBEth Fragments: " << num_wibeth_frames_count << std::endl;
  std::cout << "Total number of WIBEthFrame objects: " << num_wibeth_count << std::endl;

  return 0;
}
