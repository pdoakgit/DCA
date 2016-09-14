// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Peter Staar (taa@zurich.ibm.com)
//
// This class reads, stores, and writes the double-counting parameters.
//
// TODO: Const correctness.

#ifndef DCA_PHYS_PARAMETERS_DOUBLE_COUNTING_PARAMETERS_HPP
#define DCA_PHYS_PARAMETERS_DOUBLE_COUNTING_PARAMETERS_HPP

#include <stdexcept>
#include <string>

namespace dca {
namespace phys {
namespace params {
// dca::phys::params::

class DoubleCountingParameters {
public:
  DoubleCountingParameters() : double_counting_method_("none"), double_counting_correction_(0.) {}

  template <typename Concurrency>
  int getBufferSize(Concurrency& concurrency) const;
  template <typename Concurrency>
  void pack(Concurrency& concurrency, int* buffer, int buffer_size, int& position) const;
  template <typename Concurrency>
  void unpack(Concurrency& concurrency, int* buffer, int buffer_size, int& position);

  template <typename ReaderOrWriter>
  void readWrite(ReaderOrWriter& reader_or_writer);

  const std::string& get_double_counting_method() const {
    return double_counting_method_;
  }
  double get_double_counting_correction() const {
    return double_counting_correction_;
  }

private:
  std::string double_counting_method_;
  double double_counting_correction_;
};

template <typename Concurrency>
int DoubleCountingParameters::getBufferSize(Concurrency& concurrency) const {
  int buffer_size = 0;

  buffer_size += concurrency.get_buffer_size(double_counting_method_);
  buffer_size += concurrency.get_buffer_size(double_counting_correction_);

  return buffer_size;
}

template <typename Concurrency>
void DoubleCountingParameters::pack(Concurrency& concurrency, int* buffer, int buffer_size,
                                    int& position) const {
  concurrency.pack(buffer, buffer_size, position, double_counting_method_);
  concurrency.pack(buffer, buffer_size, position, double_counting_correction_);
}

template <typename Concurrency>
void DoubleCountingParameters::unpack(Concurrency& concurrency, int* buffer, int buffer_size,
                                      int& position) {
  concurrency.unpack(buffer, buffer_size, position, double_counting_method_);
  concurrency.unpack(buffer, buffer_size, position, double_counting_correction_);
}

template <typename ReaderOrWriter>
void DoubleCountingParameters::readWrite(ReaderOrWriter& reader_or_writer) {
  try {
    reader_or_writer.open_group("double-counting-parameters");

    try {
      reader_or_writer.execute("double-counting-method", double_counting_method_);
    }
    catch (const std::exception& r_e) {
    }
    try {
      reader_or_writer.execute("double-counting-correction", double_counting_correction_);
    }
    catch (const std::exception& r_e) {
    }

    reader_or_writer.close_group();
  }
  catch (const std::exception& r_e) {
  }
}

}  // params
}  // phys
}  // dca

#endif  // DCA_PHYS_PARAMETERS_DOUBLE_COUNTING_PARAMETERS_HPP