#pragma once

#include "types.hpp"
#include "base_attribute_vector.hpp"

namespace opossum {

// BaseAttributeVector is the abstract super class for all attribute vectors,
// e.g., FixedSizeAttributeVector
template <typename T>
class FixedSizeAttributeVector : public BaseAttributeVector {
 public:
  FixedSizeAttributeVector(const size_t number_of_elements) {
    _attribute_storage.resize(number_of_elements);
  }
  ~FixedSizeAttributeVector() override {
    
  };

  ValueID get(const size_t i) const override {
    return (ValueID) _attribute_storage.at(i);
  };

  // sets the value id at a given position
  void set(const size_t i, const ValueID value_id) override {
    _attribute_storage.at(i) = (T) value_id;
  };

  // returns the number of values
  size_t size() const override {
    return _attribute_storage.size();
  };

  // returns the width of biggest value id in bytes
  AttributeVectorWidth width() const override {
    return sizeof(T);
  };

  protected:
    std::vector<T> _attribute_storage;
};
}  // namespace opossum

