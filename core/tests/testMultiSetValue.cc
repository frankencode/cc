#include <cc/debug>
#include <cc/stdio>
#include <cc/MultiSet>

namespace cc { template class MultiSet<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    MultiSet<int> a;
    for (int i = 0; i < 10; ++i)
        a << i;

    CC_INSPECT(a);

    MultiSet<int> b = a;
    int x, y;
    a >> x >> y;

    CC_INSPECT(a);
    CC_INSPECT(b);

    CC_INSPECT(a == b);
    CC_INSPECT(a < b);
    CC_INSPECT(b < a);

    MultiSet<String> c = { "A", "A", "B", "C" };
    CC_INSPECT(c);

    return 0;
}
