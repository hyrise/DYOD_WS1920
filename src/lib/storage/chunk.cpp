#include <iomanip>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "base_segment.hpp"
#include "chunk.hpp"

#include "utils/assert.hpp"

namespace opossum {

void Chunk::add_segment(std::shared_ptr<BaseSegment> segment) {
  _columns.push_back(segment);
}

void Chunk::append(const std::vector<AllTypeVariant>& values) {
  // TODO Throw a more descriptive exception?
  if (values.size() != _columns.size())
    throw std::exception();
  for (ColumnID value_index = (ColumnID)0; value_index < (ColumnID)values.size(); ++value_index)
    _columns[value_index].get()->append(values[value_index]);
}

std::shared_ptr<BaseSegment> Chunk::get_segment(ColumnID column_id) const {
  if (column_id > _columns.size())
    return nullptr;
  return _columns[column_id];
}

uint16_t Chunk::column_count() const {
  return _columns.size();
}

uint32_t Chunk::size() const {
  if (_columns.empty())
    return 0;
  return _columns[0].get()->size();
}

}  // namespace opossum
