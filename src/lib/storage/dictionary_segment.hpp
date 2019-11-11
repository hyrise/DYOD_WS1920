#pragma once

#include <limits>
#include <memory>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

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
  explicit DictionarySegment(const std::shared_ptr<BaseSegment>& base_segment) : _dictionary(std::make_shared<std::vector<T>>()),
  _attribute_vector(std::make_shared<std::vector<uint32_t>>())
  {
    std::vector<T> value_helper;
    for (size_t i = 0; i < base_segment->size();++i){
      value_helper.push_back(type_cast<T>((*base_segment)[i]));
    }
    std::set<T> dictionary_helper;
    for (auto value : value_helper) {
      dictionary_helper.insert(value);
    }
    _dictionary->reserve(dictionary_helper.size());
    for (auto it = dictionary_helper.begin(); it != dictionary_helper.end(); ) {
      _dictionary->emplace_back(std::move(dictionary_helper.extract(it++).value()));
    }
    _attribute_vector->reserve(value_helper.size());
    for (auto value : value_helper) {
      auto dict_index = std::distance(_dictionary->begin(), std::upper_bound(_dictionary->begin(), _dictionary->end(), value));
      _attribute_vector->push_back(dict_index - 1);
    }
  };

  // SEMINAR INFORMATION: Since most of these methods depend on the template parameter, you will have to implement
  // the DictionarySegment in this file. Replace the method signatures with actual implementations.

  // return the value at a certain position. If you want to write efficient operators, back off!
  AllTypeVariant operator[](const ChunkOffset chunk_offset) const override {
    auto helper = _attribute_vector->at(chunk_offset);
    return (*_dictionary)[helper];
  };

  // return the value at a certain position.
  T get(const size_t chunk_offset) const {
    auto helper = _attribute_vector->at(chunk_offset);
    return _dictionary->at(helper);
  };

  // dictionary segments are immutable
  void append(const AllTypeVariant&) override {
    DebugAssert(1, "Dictionary Segments are immutable");
  };

  // returns an underlying dictionary
  std::shared_ptr<const std::vector<T>> dictionary() const {
    return _dictionary;
  };

  // returns an underlying data structure
  // TODO: Change back signature and implement Base Attribute Vector
  std::shared_ptr<BaseAttributeVector> attribute_vector() const{
    return _attribute_vector;
  };

  // return the value represented by a given ValueID
  const T& value_by_value_id(ValueID value_id) const {
    return _dictionary->at(value_id);
  };

  // returns the first value ID that refers to a value >= the search value
  // returns INVALID_VALUE_ID if all values are smaller than the search value
  ValueID lower_bound(T value) const {
    auto result = (ValueID) std::distance(_dictionary->begin(), std::lower_bound(_dictionary->begin(), _dictionary->end(), value));
    if (result == _dictionary->size()) {
      return INVALID_VALUE_ID;
    }
    return result;
  };

  // same as lower_bound(T), but accepts an AllTypeVariant
  ValueID lower_bound(const AllTypeVariant& value) const {
    auto result = (ValueID) std::distance(_dictionary->begin(), std::lower_bound(_dictionary->begin(), _dictionary->end(), (T) value));
    if (result == _dictionary->size()) {
      return INVALID_VALUE_ID;
    }
    return result;
  };

  // returns the first value ID that refers to a value > the search value
  // returns INVALID_VALUE_ID if all values are smaller than or equal to the search value
  ValueID upper_bound(T value) const {
    auto result = (ValueID)  std::distance(_dictionary->begin(),std::upper_bound(_dictionary->begin(), _dictionary->end(), value));
    if (result == _dictionary->size()) {
      return INVALID_VALUE_ID;
    }
    return result;
  };

  // same as upper_bound(T), but accepts an AllTypeVariant
  ValueID upper_bound(const AllTypeVariant& value) const {
    auto result = (ValueID) std::distance(_dictionary->begin(), std::upper_bound(_dictionary->begin(), _dictionary->end(), (T) value));
    if (result == _dictionary->size()) {
      return INVALID_VALUE_ID;
    }
    return result;
  };

  // return the number of unique_values (dictionary entries)
  size_t unique_values_count() const {
    return _dictionary->size();
  };

  // return the number of entries
  size_t size() const override {
    return _attribute_vector->size();
  };

  // returns the calculated memory usage
  size_t estimate_memory_usage() const;  // Bauen schlägt fehl, wenn hinter const final

 protected:
  std::shared_ptr<std::vector<T>> _dictionary;
  std::shared_ptr<std::vector<uint32_t>> _attribute_vector;
};

}  // namespace opossum
