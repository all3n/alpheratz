#pragma once
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>

namespace alpheratz {
namespace io {
template <typename ResourceType, typename DeleterType>
class ResourceGuard {
   public:
    ResourceGuard(ResourceType resource, DeleterType deleter)
        : resource_(resource), deleter_(deleter) {}

    ~ResourceGuard() {
        if (resource_) deleter_(resource_);
    }

   private:
    ResourceType resource_;
    DeleterType deleter_;
};

void SocketDeleter(int fd);
void FileDeleter(std::FILE *file);
using socket_guard = ResourceGuard<int, decltype(&SocketDeleter)>;
using file_guard = ResourceGuard<std::FILE *, decltype(&FileDeleter)>;
#define SOCKET_GUARD(s) resource_guard<int, decltype(&socket_deleter)>(s, socket_deleter);
#define FILE_GUARD(s) resource_guard<std::FILE *, decltype(&file_deleter)>(s, file_deleter);

}  // namespace io
}  // namespace alpheratz
