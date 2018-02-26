#include <cc/ui/Application>
#include <cc/ui/FontManager>
#include <cc/ui/TypeSetter>

using namespace cc;
using namespace cc::ui;

class TestView: public View
{
    friend class Object;

    TestView()
    {
        size = Size{640, 480};
        color = Color{"#FFFFFF"};

        glyphRuns_ = List< Ref<const GlyphRun> >::create();

        String pangram = "The quick brown fox jumps over the lazy dog";

        for (const FontFamily *fontFamily: FontManager::instance()->getFontFamilies()) {
            String text = Format("%%: %%") << fontFamily->name() << pangram;
            glyphRuns_
                << GlyphRun::layout(text, Font::select(fontFamily->name(), fontSize_, Weight::Normal))
                << GlyphRun::layout(text, Font::select(fontFamily->name(), fontSize_, Weight::Bold))
                << GlyphRun::layout(text, Font::select(fontFamily->name(), fontSize_, Weight::Normal, Slant::Italic));
        }
    }

    void paint() override
    {
        Painter p(this);
        p->setSource(Color{"#000000"});
        int i = 0;
        for (const GlyphRun *glyphRun: glyphRuns_) {
            p->showGlyphRun(
                Step{30, 30 + glyphRun->size()[1] + 2 * fontSize_ * i++},
                glyphRun
            );
        }
    }

    const double fontSize_ = 20;
    Ref< List< Ref<const GlyphRun> > > glyphRuns_;
};

int main(int argc, char **argv)
{
    Application *app = Application::open(argc, argv);
    String fontPath = "/usr/share/fonts/truetype/dejavu/";
    if (argc > 1) fontPath = argv[1];
    FontManager::instance()->addPath(fontPath);
    Window::open(Object::create<TestView>(), fontPath);
    return app->run();
}