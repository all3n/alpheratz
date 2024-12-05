#include <alpheratz/conf/yaml_conf.h>

#include <filesystem>
namespace fs = std::filesystem;
namespace alpheratz {
namespace conf {
absl::Status YamlConfig::Load(absl::string_view filename) {
    fs::path config_path(std::string(filename.data(), filename.size()));
    if (!fs::exists(config_path)) {
        return absl::NotFoundError(absl::StrCat(filename, " not found"));
    }
    try {
        LOG(INFO) << config_path << std::endl;
        root_ = YAML::LoadFile(config_path.string());
    } catch (const YAML::ParserException& e) {
        LOG(ERROR) << "Parse Error" << e.what();
        return absl::InternalError(absl::StrCat(e.what(), " msg:", e.msg));
    } catch (const YAML::BadFile& e) {
        LOG(ERROR) << "Bad File:" << e.what();
        return absl::InternalError(absl::StrCat(e.what(), " msg:", e.msg));
    }
    initialized_ = true;
    return absl::OkStatus();
}

}  // namespace conf
}  // namespace alpheratz
