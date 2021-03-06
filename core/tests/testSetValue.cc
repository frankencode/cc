#include <cc/debug>
#include <cc/stdio>
#include <cc/Set>

namespace cc { template class Set<int>; }

int main(int argc, char *argv[])
{
    using namespace cc;

    Set<int> a;
    for (int i = 0; i < 10; ++i)
        a << i;

    CC_INSPECT(a);

    Set<int> b = a;
    int x, y;
    a >> x >> y;

    CC_INSPECT(a);
    CC_INSPECT(b);

    CC_INSPECT(a == b);
    CC_INSPECT(a < b);
    CC_INSPECT(b < a);

    Set<String> c = { "A", "B", "C" };

    CC_INSPECT(c);

    return 0;
}
