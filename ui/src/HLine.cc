/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/HLine>

namespace cc {
namespace ui {

Ref<HLine> HLine::create(View *parent, double maxThickness)
{
    return Object::create<HLine>(parent, maxThickness);
}

HLine::HLine(View *parent, double maxThickness):
    View(parent)
{
    color = Color::Transparent;

    size->bind([=]{
        return Size { parent->size()[0], maxThickness };
    });
}

void HLine::paint()
{
    Painter p(this);
    p->rectangle(Point{ 0, 0 }, Size { size()[0], thickness() });
    p->setSource(ink());
    p->fill();
}

}} // namespace cc::ui
