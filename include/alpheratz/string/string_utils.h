#pragma once
#include <cstdio>
#include <string>
#include <vector>

namespace alpheratz {
namespace string {
/**
 * split string by string
 * line[in] : input string
 * delimiter[in]: delimiter string to split
 * out[out]: output vector string
 */
void Split(const std::string &line, const std::string &delimiter, std::vector<std::string> &out);

}  // namespace string
}  // namespace alpheratz
