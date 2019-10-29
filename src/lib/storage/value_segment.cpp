#include "value_segment.hpp"

#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "type_cast.hpp"
#include "utils/assert.hpp"
#include "utils/performance_warning.hpp"

namespace opossum {

template <typename T>
AllTypeVariant ValueSegment<T>::operator[](const ChunkOffset chunk_offset) const {
  PerformanceWarning("operator[] used");
  return (AllTypeVariant)column_data[chunk_offset];
}

template <typename T>
void ValueSegment<T>::append(const AllTypeVariant& val) {
  T data_value = type_cast<T>(val);
  column_data.push_back(data_value);
}

template <typename T>
size_t ValueSegment<T>::size() const {
  return column_data.size();
}

template <typename T>
const std::vector<T>& ValueSegment<T>::values() const {
  return column_data;
}

EXPLICITLY_INSTANTIATE_DATA_TYPES(ValueSegment);

}  // namespace opossum
