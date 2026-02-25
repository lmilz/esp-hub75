// Compilation-only TU to ensure all public headers are valid and clang-tidy can
// analyse them. This file is not meant to be run — it only needs to compile.

#include "Framebuffer.hpp"
#include "Types.hpp"

// Force template instantiation so clang-tidy sees the full body
template class hub75::Framebuffer<64, 64>;
template class hub75::Framebuffer<32, 32>;
