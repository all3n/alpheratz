#pragma once
#include <absl/status/status.h>
#include <absl/status/statusor.h>
namespace alpheratz {
namespace common {
class ResourceLoader {
   public:
    static ResourceLoader& Get() {
        static ResourceLoader instance;
        return instance;
    }
    absl::Status Load(absl::string_view path);
    absl::StatusOr<std::vector<uint8_t>*> GetResource(const std::string& path);

   private:
    ResourceLoader() {}
    std::unordered_map<std::string, std::vector<uint8_t>> resource_map_;
};

}  // namespace common
}  // namespace alpheratz
