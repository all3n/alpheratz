#include <alpheratz/common/resource_loader.h>
#include <glog/logging.h>

#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
namespace alpheratz {
namespace common {

absl::Status ResourceLoader::Load(absl::string_view path) {
    std::vector<std::string> out;
    std::function<void(std::string &)> loop_file = [&](std::string &input_path) {
        for (fs::directory_iterator it(input_path); it != fs::directory_iterator(); ++it) {
            if (fs::is_regular_file(it->status())) {
                out.push_back(fs::absolute(it->path()).string());
            } else if (fs::is_directory(it->status())) {
                std::string path = it->path().string();
                loop_file(path);
            }
        }
    };
    std::string base_path_str(path.data(), path.size());
    fs::path base_path(fs::absolute(base_path_str));
    std::string bash_path_str = base_path.string();
    loop_file(base_path_str);

    // LOAD
    for (const auto &file : out) {
        std::string relative_name = file.substr(base_path_str.size());
        LOG(INFO) << file << ": " << relative_name;
        std::ifstream ifs(file, std::ios::binary | std::ios::in);
        ifs.seekg(0, std::ios::end);
        size_t file_size = ifs.tellg();
        auto &res = resource_map_[relative_name];
        res.resize(file_size);
        ifs.seekg(0, std::ios::beg);
        ifs.read(reinterpret_cast<char *>(res.data()), file_size);
        ifs.close();
    }
    return absl::OkStatus();
}
absl::StatusOr<std::vector<uint8_t> *> ResourceLoader::GetResource(const std::string &path) {
    auto it = resource_map_.find(path);
    if (it == resource_map_.end()) {
        return absl::NotFoundError(path + " not found");
    }
    return &(it->second);
}
}  // namespace common
}  // namespace alpheratz
