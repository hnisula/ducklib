#ifndef RHI_H
#define RHI_H
#include "swap_chain.h"

namespace ducklib::render {
class Rhi {
public:
    auto create_swap_chain() -> SwapChain;
};
}

#endif //RHI_H
