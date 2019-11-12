#pragma once

#include <memory>
#include <string>

#include "all_type_variant.hpp"
#include "types.hpp"

namespace opossum {

// BaseSegment is the abstract super class for all segment types,
// e.g., ValueSegment, ReferenceSegment
class VariableWidthAttributeVector : private Noncopyable {
 public:
  VariableWidthAttributeVector();
  ~VariableWidthAttributeVector();

  // we need to explicitly set the move constructor to default when
  // we overwrite the copy constructor
  VariableWidthAttributeVector(VariableWidthAttributeVector&&) = default;
  VariableWidthAttributeVector& operator=(VariableWidthAttributeVector&&) = default;

  // returns the value at a given position
  AllTypeVariant operator[](const ChunkOffset chunk_offset);

  // appends the value at the end of the segment
  void append(const AllTypeVariant& val);

  // returns the number of values
  size_t size();

  // returns the calculated memory usage
  // virtual size_t estimate_memory_usage() const;  //TODO: Schlägt fehl beim Bauen
};
}  // namespace opossum
