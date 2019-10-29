#include "storage_manager.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "utils/assert.hpp"

namespace opossum {

StorageManager& StorageManager::get() {
  static StorageManager instance;
  return instance;
}

void StorageManager::add_table(const std::string& name, std::shared_ptr<Table> table) {
  tables.insert_or_assign(name, table);
}

void StorageManager::drop_table(const std::string& name) {
  auto search = tables.find(name);
  if (search == tables.end()){
    throw std::exception();
  } 
  tables.erase(name);
}

std::shared_ptr<Table> StorageManager::get_table(const std::string& name) const {
  auto search = tables.find(name);
  if(search != tables.end()){
    return search->second;
  }

  throw std::exception();
}

bool StorageManager::has_table(const std::string& name) const {
  auto search = tables.find(name);
  return search != tables.end();
}

std::vector<std::string> StorageManager::table_names() const {
  std::vector<std::string> names;
  for(auto pair: tables){
    names.push_back(pair.first);
  }
  return names;
}

void StorageManager::print(std::ostream& out) const {
  for(auto pair: tables){
    out << pair.first << " | " << pair.second->column_count() << " | " << pair.second->row_count() << " | " << pair.second->chunk_count();
  }
}

void StorageManager::reset() {
  tables.clear();
}

}  // namespace opossum
