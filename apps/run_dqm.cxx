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

#include "daqdataformats/Fragment.hpp"
#include "daqdataformats/RecordID.hpp"
#include "daqdataformats/SourceID.hpp"
#include "fddetdataformats/WIBEthFrame.hpp"
#include "rawdatafilelibs/BackendFactory.hpp"
#include "rawdatafilelibs/RawDataFileReader.hpp"

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
 * @brief Opens a raw data file (any format with a rawdatafilelibs backend, e.g. HDF5)
 * 
 * @param _filename string with the filename
 * @return std::shared_ptr<rawdatafilelibs::RawDataFileReader> pointer to the file reader
 */
std::shared_ptr<rawdatafilelibs::RawDataFileReader>
get_file(const std::string& _filename)
{
  std::shared_ptr<rawdatafilelibs::RawDataFileReader> file_hdf5 = rawdatafilelibs::open_raw_data_file(_filename);
  if (file_hdf5->get_record_type() != rawdatafilelibs::RecordType::kTriggerRecord) {
    throw std::runtime_error(fmt::format("ERROR: input file '{}' not of type 'TriggerRecord'", _filename));
  }

  std::string application_name = file_hdf5->get_metadata().application_name;
  std::cout << "File '" << _filename << "' has application name: " << application_name << std::endl;

  return file_hdf5;
}

std::pair<uint64_t, uint64_t>
get_available_slice_id_range(const std::shared_ptr<rawdatafilelibs::RawDataFileReader>& _file)
{
  if (!_file) {
    throw std::runtime_error("No valid HDF5 object provided");
  }

  // Find min/max record id for this application
  auto const& record_ids = _file->get_record_ids();
  if (record_ids.empty()) {
    throw std::runtime_error("HDF5 object contains no records.");
  }
  uint64_t start = record_ids.begin()->number;
  uint64_t end = record_ids.rbegin()->number;

  std::cout << "RecordID range: [" << start << "," << end << "]" << std::endl;

  return {start, end};
}

uint64_t
get_wibeth_count(const std::shared_ptr<rawdatafilelibs::RawDataFileReader>& _file_hdf5,
                 const daqdataformats::RecordID& _record_id,
                 const daqdataformats::SourceID& _source_id)
{
  // Get the fragment and print its type
  std::unique_ptr<daqdataformats::Fragment> frag_ptr = _file_hdf5->get_fragment(_record_id, _source_id);
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
  std::shared_ptr<rawdatafilelibs::RawDataFileReader> file_hdf5 = 
    get_file(opts.input_file);

  // Get the available record_id range
  std::pair<uint64_t, uint64_t> recordid_range = get_available_slice_id_range(file_hdf5);
  uint64_t recordid_to_process = recordid_range.first;

  uint64_t num_wibeth_frames_count = 0;
  uint64_t num_wibeth_count = 0;

  // As before, the record with that number and sequence number 0
  const daqdataformats::RecordID record_id(recordid_to_process, 0);
  for (const daqdataformats::SourceID& source_id : file_hdf5->get_fragment_source_ids(record_id)) {
    std::cout << "Fragment SourceID: " << source_id.to_string() << "\n";

    // I love this new C++ feature
    if (uint64_t num_wibs = get_wibeth_count(file_hdf5, record_id, source_id)){
      num_wibeth_count += num_wibs;
      num_wibeth_frames_count++;
    }
  }

  std::cout << "Total number of WIBEth Fragments: " << num_wibeth_frames_count << std::endl;
  std::cout << "Total number of WIBEthFrame objects: " << num_wibeth_count << std::endl;

  return 0;
}
