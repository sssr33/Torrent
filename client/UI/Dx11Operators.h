#include "Dx11Include.h"

namespace Dx11Operators {
    inline bool operator==(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b) {
        return
            a.a == b.a &&
            a.b == b.b &&
            a.g == b.g &&
            a.r == b.r;
    }
}
