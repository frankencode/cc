#include <cc/ui/ScrollView>
#include <cc/ui/ColumnLayout>
#include <cc/ui/TextField>
#include <cc/ui/PasswordField>

using namespace cc;
using namespace cc::ui;

class MainView: public View
{
    friend class Object;

    MainView()
    {
        size = Size { 640, 480 };
        inheritPaper();

        auto carrier = add<ScrollView>()->carrier();

        ColumnLayout column{carrier};
        column->margin = dp(12);
        column->spacing = dp(12);

        carrier->add<TextField>("First name");
        carrier->add<TextField>("Middle name");
        carrier->add<TextField>("Last name");
        carrier->add<TextField>("Age");
        carrier->add<PasswordField>("Enter password");
        carrier->add<PasswordField>("Repeat password");
        carrier->add<TextField>("Street address");
        carrier->add<TextField>("Zip code");
        carrier->add<TextField>("Country");
    }
};

int main(int argc, char **argv)
{
    Window::open<MainView>("Hello, world!", WindowMode::Accelerated|WindowMode::VSync);
    return Application{}->run();
}
