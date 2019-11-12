#pragma once

#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "all_type_variant.hpp"
#include "type_cast.hpp"
#include "types.hpp"

namespace opossum {

class BaseAttributeVector;
class BaseSegment;

// Even though ValueIDs do not have to use the full width of ValueID (uint32_t), this will also work for smaller ValueID
// types (uint8_t, uint16_t) since after a down-cast INVALID_VALUE_ID will look like their numeric_limit::max()
constexpr ValueID INVALID_VALUE_ID{std::numeric_limits<ValueID::base_type>::max()};

// Dictionary is a specific segment type that stores all its values in a vector
template <typename T>
class DictionarySegment : public BaseSegment {
 public:
  /**
   * Creates a Dictionary segment from a given value segment.
   */
  explicit DictionarySegment(const std::shared_ptr<BaseSegment>& base_segment) {
    //DebugAssert(base_segment->size() <= std::numeric_limits<ChunkOffset>::max(), "too many values in a segment");
    const auto size = base_segment->size();
    _dictionary = std::make_shared<std::vector<T>>();

    if (size < std::numeric_limits<uint32_t>::max()) {
      // create a new _dictionary
      std::set<T> dic_set;
      for (size_t base_index = 0; base_index < size; ++base_index) {
        const auto& value = type_cast<T>((*base_segment)[base_index]);
        dic_set.insert(value);
      }

      _dictionary->resize(dic_set.size());
      for (auto it = dic_set.begin(); it != dic_set.end(); ++it) {
        _dictionary->emplace_back(std::move(dic_set.extract(++it).value()));
      }
    }

      auto num_values = base_segment->size();
    _attribute_vector.make_shared(num_values, INVALID_VALUE_ID);
      //_attribute_vector = std::make_shared<BaseAttributeVector>(num_values, INVALID_VALUE_ID);
      for (uint32_t dic_index = 0; dic_index < num_values; dic_index++) {
          //const auto& value = type_cast<T>((*base_segment)[dic_index]);
          //auto value_id = lower_bound(value);
          //_attribute_vector->set(dic_index, value_id);
          _attribute_vector->push_back(dic_index);
      }
  }

  // SEMINAR INFORMATION: Since most of these methods depend on the template parameter, you will have to implement
  // the DictionarySegment in this file. Replace the method signatures with actual implementations.

  // return the value at a certain position. If you want to write efficient operators, back off!
  AllTypeVariant operator[](const ChunkOffset chunk_offset) const override {
    return -1;
    //return *(AllTypeVariant)_dictionary(chunk_offset);
  }

  // return the value at a certain position.
  T get(const size_t chunk_offset) const {
    if (_dictionary.get() == &chunk_offset) {
      return *_dictionary.get();
    }
    return 0;
  }

  // dictionary segments are immutable
  void append(const AllTypeVariant&) override {}

  // returns an underlying dictionary
  std::shared_ptr<const std::vector<T>> dictionary() const { return _dictionary; }

  // returns an underlying data structure
  std::shared_ptr<const BaseAttributeVector> attribute_vector() const { return _attribute_vector; }

  // return the value represented by a given ValueID
  const T& value_by_value_id(ValueID value_id) const;

  // returns the first value ID that refers to a value >= the search value
  // returns INVALID_VALUE_ID if all values are smaller than the search value
  ValueID lower_bound(T value) const {
      auto iter = std::lower_bound(_dictionary->begin(), _dictionary->end(), value);
      if (iter < _dictionary->end()) {
          return INVALID_VALUE_ID;
      }
      return ValueID{iter - _dictionary->begin()};
  }

  // same as lower_bound(T), but accepts an AllTypeVariant
  ValueID lower_bound(const AllTypeVariant& value) const {
      auto iter = std::lower_bound(_dictionary->begin(), _dictionary->end(), value);
      if (iter < _dictionary->end()) {
          return INVALID_VALUE_ID;
      }
      return ValueID{iter - _dictionary->begin()};
  }

  // returns the first value ID that refers to a value > the search value
  // returns INVALID_VALUE_ID if all values are smaller than or equal to the search value
  ValueID upper_bound(T value) const {
      auto iter = std::upper_bound(_dictionary->begin(), _dictionary->end(), value);
      if (iter <= _dictionary->end()) {
          return INVALID_VALUE_ID;
      }
      return ValueID{iter - _dictionary->begin()};
  }

  // same as upper_bound(T), but accepts an AllTypeVariant
  ValueID upper_bound(const AllTypeVariant& value) const {
      auto iter = std::upper_bound(_dictionary->begin(), _dictionary->end(), value);
      if (iter <= _dictionary->end()) {
          return INVALID_VALUE_ID;
      }
      return ValueID{iter - _dictionary->begin()};
  }

  // return the number of unique_values (dictionary entries)
  size_t unique_values_count() const {
    return _dictionary->size();
  }

  // return the number of entries
  size_t size() const override { return _dictionary->size(); }

  // returns the calculated memory usage
  // Bauen schlägt fehl, wenn hinter const final
  size_t estimate_memory_usage() const {
    auto num_entries = _dictionary.size();
    auto data_type_size = sizeof(T);
    return sizeof(*this) + num_entries * data_type_size;

  }

 protected:
  std::shared_ptr<std::vector<T>> _dictionary;
  //std::shared_ptr<BaseAttributeVector> _attribute_vector;
  std::shared_ptr<std::vector<uint32_t>> _attribute_vector;
};

}  // namespace opossum