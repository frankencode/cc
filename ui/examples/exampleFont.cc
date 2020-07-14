#include <cc/ui/ScrollView>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size{640, 480};
        inheritPaper();

        auto scroll = ScrollView::create(this);
        ColumnLayout{scroll->carrier()};

        for (int size = 16; size <= 64; size += 8) {
            String text = "A small brown fox... (" + str(size) + ")";
            scroll->add<Label>(text, Font(size));
            scroll->add<Label>(text, Font(size) << Slant::Italic);
            scroll->add<Label>(text, Font(size) << Weight::Bold);
            scroll->add<Label>(text, Font(size) << Slant::Italic << Weight::Bold);
        }
    }
};

int main(int argc, char **argv)
{
    Window::open<MainView>("Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}
