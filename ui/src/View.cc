/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/View>
#include <cc/ui/Application>
#include <cc/ui/StyleManager>
#include <cc/ui/Window>
#include <cc/ui/UpdateRequest>
#include <cc/ui/Control>

namespace cc {
namespace ui {

View::Instance::Instance()
{
    pos >>[=]{ update(UpdateReason::Moved); };
    angle >>[=]{ update(UpdateReason::Moved); };
    scale >>[=]{ update(UpdateReason::Moved); };

    visible >>[=]{
        for (int i = 0, n = childCount(); i < n; ++i)
            childAt(i)->visible = visible();
        if (!visible()) {
            context_ = nullptr;
            image_ = Image{};
            if (parentInstance())
                parentInstance()->visibleChildren_->remove(serial_);
            update(UpdateReason::Hidden);
        }
        else {
            if (parentInstance())
                parentInstance()->visibleChildren_->insert(serial_, this);
            paint();
        }
    };

    paint <<[=]{ if (isPainted()) Painter{this}; };

    paint >>[=]{ update(UpdateReason::Changed); };
}

View::Instance::~Instance()
{
    layout_ = Layout{nullptr};
        // destroy the layout before releasing the children for efficiency
}

void View::Instance::disband()
{
    #if 0 // FIXME: review cleanup strategy
    for (auto &item: children_)
        item->value()->disband();

    build->disband();
    paint->disband();

    visible->disband();
    moving->disband();
    paper->disband();

    pos->disband();
    size->disband();
    padding->disband();

    center->disband();
    angle->disband();
    scale->disband();
    childCount->disband();
    parentInstance->disband();

    layout_ = Layout{nullptr};
    #endif
}

Point View::Instance::mapToGlobal(Point l) const
{
    for (const Instance *h = this; h->parentInstance(); h = h->parentInstance())
        l += h->pos();
    return l;
}

Point View::Instance::mapToLocal(Point g) const
{
    for (const Instance *h = this; h->parentInstance(); h = h->parentInstance())
       g -= h->pos();
    return g;
}

Point View::Instance::mapToChild(const Instance *child, Point l) const
{
    for (const Instance *h = child; h != this && h->parentInstance(); h = h->parentInstance())
        l -= h->pos();
    return l;
}

Point View::Instance::mapToParent(const Instance *parent, Point l) const
{
    for (const Instance *h = this; h != parent && h->parentInstance(); h = h->parentInstance())
        l += h->pos();
    return l;
}

bool View::Instance::withinBounds(Point l) const
{
    return
        0 <= l[1] && l[1] < size()[1] &&
        0 <= l[0] && l[0] < size()[0];
}

View View::Instance::getChildAt(Point l) /// \todo replace by a safer version: bool lookupChildAt(Point l, View *view) const;
{
    for (auto pair: visibleChildren_) {
        View child = pair->value();
        if (child->containsLocal(mapToChild(child, l))) return child;
    }
    return View{nullptr};
}

Control View::Instance::getControlAt(Point l) /// \todo replace by a safer version: bool lookupControlAt(Point l, Control *view) const;
{
    for (auto pair: visibleChildren_) {
        View child = pair->value();
        if (child->containsLocal(mapToChild(child, l))) {
            Control control = child->as<Control>();
            if (control) {
                while (control->delegate())
                    control = control->delegate();
                return control;
            }
        }
    }
    return Control{nullptr};
}

bool View::Instance::isParentOf(const Instance *other) const
{
    for (const Instance *h = other; h; h = h->parentInstance()) {
        if (h == this)
            return true;
    }
    return false;
}

bool View::Instance::isFullyVisibleIn(const Instance *other) const
{
    if (!other) return false;
    if (other == this) return true;
    if (!other->isParentOf(this)) return false;

    return
        other->withinBounds(mapToParent(other, Point{})) &&
        other->withinBounds(mapToParent(other, size() - Size{1, 1}));
}

void View::Instance::centerInParent()
{
    pos <<[=]{ return parentInstance() ? 0.5 * (parentInstance()->size() - size()) : Point{}; };
}

Color View::Instance::basePaper() const
{
    for (const Instance *h = parentInstance(); h; h = h->parentInstance()) {
        if (h->paper())
            return h->paper();
    }

    return style()->theme()->windowColor();
}

void View::Instance::inheritPaper()
{
    paper <<[=]{ return basePaper(); };
}

bool View::Instance::isOpaque() const
{
    return paper()->isOpaque();
}

bool View::Instance::isPainted() const
{
    return paper()->isValid() && size()[0] > 0 && size()[1] > 0;
}

bool View::Instance::isStatic() const
{
    return false; // FIXME
}

void View::Instance::clear(Color c)
{
    image()->clear(c->premultiplied());
}

void View::Instance::clear()
{
    clear(paper());
}

void View::Instance::update(UpdateReason reason)
{
    Window *w = window();
    if (!w) return;

    if (!visible() && reason != UpdateReason::Hidden) return;

    w->addToFrame(UpdateRequest::create(reason, this));
}

void View::Instance::childReady(View &child)
{
    if (layout_) layout_->childReady(child);
}

void View::Instance::childDone(View &child)
{
    if (layout_) layout_->childDone(child);
}

StylePlugin *View::Instance::style() const
{
    return StyleManager::instance()->activePlugin();
}

const Theme *View::Instance::theme() const
{
    return style()->theme();
}

Window *View::Instance::window() const
{
    if (!window_) {
        if (parentInstance())
            return parentInstance()->window();
    }
    return window_;
}

Image::Instance *View::Instance::image()
{
    if (!image_ || (
        image_->width()  != std::ceil(size()[0]) ||
        image_->height() != std::ceil(size()[1])
    ))
        image_ = Image{int(std::ceil(size()[0])), int(std::ceil(size()[1]))};

    return image_;
}

void View::Instance::insertChild(View child)
{
    child->parentInstance = this;
    child->serial_ = nextSerial();
    children_->insert(child->serial_, child);
    child->build();
    if (child->visible())
        visibleChildren_->insert(child->serial_, child);
    childCount += 1;
    childReady(child);
}

void View::Instance::removeChild(View child)
{
    View hook = child;
    children_->remove(child->serial_);
    if (child->visible())
        visibleChildren_->remove(child->serial_);
    child->serial_ = 0;
    childCount -= 1;
    childDone(child);
}

View View::Instance::setLayout(const Layout &layout)
{
    layout_ = layout;
    return this;
}

void View::Instance::adoptChild(View parent, View child)
{
    if (child->parent() != parent) {
        if (child->parent()) child->parent()->removeChild(child);
        parent->insertChild(child);
    }
}

View View::Instance::adoptLayout(View &view, const Layout &layout)
{
    return view->setLayout(layout);
}

bool View::Instance::feedFingerEvent(FingerEvent *event)
{
    {
        PointerEvent::PosGuard guard{event, mapToLocal(window()->size() * event->pos())};

        if (event->action() == PointerAction::Pressed)
        {
            if (pointerPressed(event) || fingerPressed(event))
                return true;
        }
        else if (event->action() == PointerAction::Released)
        {
            bool eaten = pointerReleased(event) || fingerReleased(event);

            if (Application{}->pressedControl()) {
                if (
                    Application{}->pressedControl()->pointerClicked(event) ||
                    Application{}->pressedControl()->fingerClicked(event)
                )
                    eaten = true;
            }

            if (eaten) return true;
        }
        else if (event->action() == PointerAction::Moved)
        {
            if (pointerMoved(event) || fingerMoved(event))
                return true;
        }
    }

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->containsGlobal(event->pos()))
        {
            if (child->feedFingerEvent(event))
                return true;
        }
    }

    return false;
}

bool View::Instance::feedMouseEvent(MouseEvent *event)
{
    {
        PointerEvent::PosGuard guard{event, mapToLocal(event->pos())};

        if (event->action() == PointerAction::Pressed)
        {
            if (pointerPressed(event) || mousePressed(event))
                return true;
        }
        else if (event->action() == PointerAction::Released)
        {
            bool eaten = pointerReleased(event) || mouseReleased(event);

            if (Application{}->pressedControl()) {
                if (
                    Application{}->pressedControl()->pointerClicked(event) ||
                    Application{}->pressedControl()->mouseClicked(event)
                )
                    eaten = true;
            }

            if (eaten) return true;
        }
        else if (event->action() == PointerAction::Moved)
        {
            if (
                (event->button() != MouseButton::None && pointerMoved(event)) ||
                mouseMoved(event)
            )
                return true;
        }
    }

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->containsGlobal(event->pos()))
        {
            if (child->feedMouseEvent(event))
                return true;
        }
    }

    return false;
}

bool View::Instance::feedWheelEvent(WheelEvent *event)
{
    if (containsGlobal(event->mousePos()))
    {
        if (wheelMoved(event))
            return true;
    }

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->containsGlobal(event->mousePos()))
        {
            if (child->feedWheelEvent(event))
                return true;
        }
    }

    return false;
}

bool View::Instance::feedKeyEvent(KeyEvent *event)
{
    if (event->action() == KeyAction::Pressed)
    {
        if (keyPressed(event)) return true;
    }
    else if (event->action() == KeyAction::Released)
    {
        if (keyReleased(event)) return true;
    }

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->feedKeyEvent(event))
            return true;
    }

    return false;
}

bool View::Instance::feedExposedEvent()
{
    if (isPainted()) paint();

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->feedExposedEvent())
            return true;
    }

    return false;
}

bool View::Instance::feedEnterEvent()
{
    windowEntered();

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->feedEnterEvent())
            return true;
    }

    return false;
}

bool View::Instance::feedLeaveEvent()
{
    windowLeft();

    for (auto pair: visibleChildren_)
    {
        View child = pair->value();

        if (child->feedLeaveEvent())
            return true;
    }

    return false;
}

uint64_t View::Instance::nextSerial() const
{
    return (children_->count() > 0) ? children_->at(children_->count() - 1)->key() + 1 : 1;
}

cairo_surface_t *View::Instance::cairoSurface() const
{
    return const_cast<Instance *>(this)->image()->cairoSurface();
}

}} // namespace cc::ui
