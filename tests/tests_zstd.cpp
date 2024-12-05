#include <absl/time/clock.h>
#include <absl/time/time.h>
#include <zstd.h>

#include <cstdio>
#include <cstring>
#include <iostream>
int main(int /*argc*/, char *argv[]) {
    FILE *fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    uint64_t raw_size = ftell(fp);
    std::cout << raw_size << std::endl;
    fseek(fp, 0, SEEK_SET);
    std::vector<char> raw_data(raw_size);
    size_t read_size = fread(raw_data.data(), 1, raw_size, fp);
    (void)read_size;
    fclose(fp);

    size_t out_bound = ZSTD_compressBound(raw_size);
    std::vector<char> compress_data(out_bound);

    absl::Time start = absl::Now();
    size_t compressed_size = ZSTD_compress(compress_data.data(), out_bound, raw_data.data(),
                                           raw_size, ZSTD_defaultCLevel());
    absl::Time end = absl::Now();
    absl::Duration duration = end - start;
    double milliseconds = absl::ToDoubleMilliseconds(duration);
    std::cout << compressed_size << std::endl;
    std::cout << milliseconds << "ms" << std::endl;

    size_t de_compress_size = ZSTD_getFrameContentSize(compress_data.data(), compressed_size);
    std::cout << de_compress_size << std::endl;

    raw_data.clear();
    raw_data.resize(de_compress_size);

    start = absl::Now();
    size_t raw_uncompress_size =
        ZSTD_decompress(raw_data.data(), de_compress_size, compress_data.data(), compressed_size);
    duration = absl::Now() - start;
    std::cout << "de_compress: " << absl::ToDoubleMilliseconds(duration) << "ms" << std::endl;

    if (auto code = ZSTD_isError(raw_uncompress_size)) {
        std::cout << ZSTD_getErrorName(code) << std::endl;
    }

    return 0;
}
