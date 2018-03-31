#include <cc/ui/Application>
#include <cc/ui/ColumnLayout>
#include <cc/ui/StylePlugin>
#include <cc/ui/Label>

using namespace cc;
using namespace cc::ui;

class MainView: public View, private KeyInput
{
    friend class Object;

    MainView():
        KeyInput(this)
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        View *box = View::create(this);
        box->color = Color{"#D0D0FF"};
        box->centerInParent();

        ColumnLayout::setup(box);

        Label::create(box, "• Item 1");
        Label::create(box, "• Item 2");
        Label::create(box, "• Item 3");

        {
            View *subBox = View::create(box);
            subBox->color = Color{"#D0FFD0"};

            ColumnLayout::setup(subBox)->indent->bind([=]{
                return (
                    StylePlugin::instance()->defaultTextStyle()->font()->size() +
                    Application::instance()->textZoom()
                );
            });

            Label::create(subBox, "◦ Item A");
            Label::create(subBox, "◦ Item B");
            Label::create(subBox, "◦ Item C");
        }

        keyPressed->connect([=]{
            if (+(key()->modifiers() & KeyModifier::Control)) {
                if (key()->keyCode() == '+') {
                    Application::instance()->textZoom += 4;
                }
                else if (key()->keyCode() == '-') {
                    Application::instance()->textZoom -= 4;
                }
            }
        });
    }
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    app->textZoom = 20;
    Window::open(Object::create<MainView>(), "Hello, world!");
    return app->run();
}
