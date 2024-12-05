#include <alpheratz/io/resource_guard.h>

#include <cstdio>

namespace alpheratz {
namespace io {

void SocketDeleter(int fd) { ::close(fd); }
void FileDeleter(std::FILE *file) { std::fclose(file); }

}  // namespace io
}  // namespace alpheratz
