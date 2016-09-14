// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Giovanni Balduzzi (gbalduzz@itp.phys.ethz.ch)
//         Urs R. Haehner (haehneru@itp.phys.ethz.ch)
//
// This file tests MCI_parameters.hpp
//
// TODO: Add tests for get_buffer_size, pack, unpack and writing.

#include "dca/phys/parameters/mci_parameters.hpp"
#include <algorithm>  // for std::sort and std::unique
#include <fstream>
#include <limits>
#include <vector>
#include "gtest/gtest.h"
#include "comp_library/IO_library/JSON/JSON.hpp"

TEST(MciParametersTest, DefaultValues) {
  dca::phys::params::MciParameters pars;

  EXPECT_EQ("zero", pars.get_Sigma_file());
  EXPECT_EQ(20, pars.get_warm_up_sweeps());
  EXPECT_EQ(1., pars.get_number_of_sweeps_per_measurement());
  EXPECT_EQ(100, pars.get_number_of_measurements());
  EXPECT_EQ(false, pars.adjust_self_energy_for_double_counting());
  EXPECT_EQ(985456376, pars.get_seed());
  EXPECT_EQ(1, pars.get_nr_walkers());
  EXPECT_EQ(1, pars.get_nr_accumulators());
  EXPECT_EQ(1, pars.get_additional_steps());
  EXPECT_EQ(1, pars.get_nr_HTS_threads());
}

TEST(MciParametersTest, ReadAll) {
  IO::reader<IO::JSON> reader;
  dca::phys::params::MciParameters pars;

  reader.open_file(DCA_SOURCE_DIR "/test/unit/phys/parameters/mci_parameters/input_read_all.json");
  pars.readWrite(reader);
  reader.close_file();

  EXPECT_EQ("sigma-file.hdf5", pars.get_Sigma_file());
  EXPECT_EQ(99, pars.get_warm_up_sweeps());
  EXPECT_EQ(1.5, pars.get_number_of_sweeps_per_measurement());
  EXPECT_EQ(47, pars.get_number_of_measurements());
  EXPECT_EQ(true, pars.adjust_self_energy_for_double_counting());
  EXPECT_EQ(123, pars.get_seed());
  EXPECT_EQ(4, pars.get_nr_walkers());
  EXPECT_EQ(5, pars.get_nr_accumulators());
  EXPECT_EQ(6, pars.get_additional_steps());
  EXPECT_EQ(7, pars.get_nr_HTS_threads());
}

TEST(MciParametersTest, ReadPositiveIntegerSeed) {
  IO::reader<IO::JSON> reader;
  dca::phys::params::MciParameters pars;

  reader.open_file(DCA_SOURCE_DIR
                   "/test/unit/phys/parameters/mci_parameters/input_pos_int_seed.json");
  pars.readWrite(reader);
  reader.close_file();

  EXPECT_EQ(42, pars.get_seed());
}

TEST(MciParametersTest, ReadNegativeIntegerSeed) {
  IO::reader<IO::JSON> reader;
  dca::phys::params::MciParameters pars;

  reader.open_file(DCA_SOURCE_DIR
                   "/test/unit/phys/parameters/mci_parameters/input_neg_int_seed.json");
  pars.readWrite(reader);
  reader.close_file();

  EXPECT_EQ(-1, pars.get_seed());
}

TEST(MciParametersTest, ReadTooLargeSeed) {
  // Generate an input file that contains a number that is larger than the maximum value of int.
  const int max = std::numeric_limits<int>::max();
  std::ofstream input;
  input.open("input_too_large_seed.json");
  input << "{\n"
        << "    \"Monte-Carlo-Integration\": {\n"
        << "        \"RNG-seed\": " << max << "0\n"  // Writes max * 10.
        << "    }\n"
        << "}\n";
  input.close();

  IO::reader<IO::JSON> reader;
  dca::phys::params::MciParameters pars;

  reader.open_file("input_too_large_seed.json");
  pars.readWrite(reader);
  reader.close_file();

  EXPECT_EQ(max, pars.get_seed());
}

TEST(MciParametersTest, ReadTooSmallSeed) {
  // Generate an input file that contains a number that is smaller than the minimum value of int.
  const int min = std::numeric_limits<int>::min();
  std::ofstream input;
  input.open("input_too_small_seed.json");
  input << "{\n"
        << "    \"Monte-Carlo-Integration\": {\n"
        << "        \"RNG-seed\": " << min << "0\n"  // Writes min * 10.
        << "    }\n"
        << "}\n";
  input.close();

  IO::reader<IO::JSON> reader;
  dca::phys::params::MciParameters pars;

  reader.open_file("input_too_small_seed.json");
  pars.readWrite(reader);
  reader.close_file();

  EXPECT_EQ(min, pars.get_seed());
}

TEST(MciParametersTest, RandomSeed) {
  // The input file contains the seeding option "random" instead of a number.
  IO::reader<IO::JSON> reader;
  dca::phys::params::MciParameters pars;

  reader.open_file(DCA_SOURCE_DIR
                   "/test/unit/phys/parameters/mci_parameters/input_random_seed.json");

  std::vector<int> seeds;
  const int n_seeds = 5;

  for (int i = 0; i < n_seeds; i++) {
    pars.readWrite(reader);
    seeds.push_back(pars.get_seed());
  }

  reader.close_file();

  // Check that the seeds are in the expected interval.
  for (int i = 0; i < n_seeds; i++) {
    EXPECT_GE(seeds[i], 0);
    EXPECT_LE(seeds[i], std::numeric_limits<int>::max());
  }

  // Check that varying seeds are generated.
  std::sort(seeds.begin(), seeds.end());
  auto new_end = std::unique(seeds.begin(), seeds.end());
  EXPECT_GT(new_end - seeds.begin(), 1);
}

TEST(MciParametersTest, InvalidSeedingOption) {
  IO::reader<IO::JSON> reader;
  dca::phys::params::MciParameters pars;

  reader.open_file(DCA_SOURCE_DIR
                   "/test/unit/phys/parameters/mci_parameters/input_invalid_seeding_option.json");
  pars.readWrite(reader);
  reader.close_file();

  EXPECT_EQ(985456376, pars.get_seed());
}