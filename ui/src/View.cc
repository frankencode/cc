/*
 * Copyright (C) 2017-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/List>
#include <cc/ui/Application>
#include <cc/ui/Window>
#include <cc/ui/Image>
#include <cc/ui/TouchEvent>
#include <cc/ui/MouseEvent>
#include <cc/ui/MouseWheelEvent>
#include <cc/ui/KeyEvent>
#include <cc/ui/View>

namespace cc {
namespace ui {

View::View(View *parent):
    serial_(0),
    window_(0),
    parent_(0),
    children_(Children::create())
{
    if (parent)
    {
        parent->insertChild(this);

        pos->connect([=]{
            if (visible()) update(UpdateReason::Moved);
        });
    }
    else {
        pos->restrict([](Point&, Point) { return false; });
    }

    size ->connect([=]{ update(UpdateReason::Resized); });
    color->connect([=]{ update(UpdateReason::Changed); });
    angle->connect([=]{ update(UpdateReason::Moved); });

    visible->connect([=]{
        update(visible() ? UpdateReason::Shown : UpdateReason::Hidden);
    });
}

View::~View()
{}

void View::disband()
{
    CC_ASSERT2(parent_, "Cannot manually destroy the top-level view");
    if (!parent_) return;
    visible = false;
    parent_->removeChild(this);
}

Point View::mapToGlobal(Point l) const
{
    for (const View *view = this; view->parent_; view = view->parent_)
        l += view->pos();
    return l;
}

Point View::mapToLocal(Point g) const
{
    for (const View *view = this; view->parent_; view = view->parent_)
       g -= view->pos();
    return g;
}

bool View::containsGlobal(Point g) const
{
    Point globalPos = mapToGlobal(Point{0, 0});
    return
        globalPos[0] <= g[0] && g[0] < globalPos[0] + size()[0] &&
        globalPos[1] <= g[1] && g[1] < globalPos[1] + size()[1];
}

void View::centerInParent()
{
    if (parent())
        pos->bind([=]{ return 0.5 * (parent()->size() - size()); });
}

bool View::isOpaque() const
{
    return Color::isOpaque(color());
}

bool View::isPainted() const
{
    return Color::isValid(color());
}

bool View::isStatic() const
{
    return true;
}

void View::clear()
{
    image()->clear(Color::premultiplied(color()));
}

void View::paint()
{}

void View::polish(Window *window)
{
    for (int i = 0; i < children_->count(); ++i)
        children_->valueAt(i)->polish(window);

    clear();
    paint();
    window->addToFrame(UpdateRequest::create(UpdateReason::Changed, this));
}

void View::update(UpdateReason reason)
{
    Window *w = window();
    if (!w) return;

    if (
        isPainted() &&
        (reason == UpdateReason::Changed || reason == UpdateReason::Resized)
    ) {
        clear();
        paint();
    }

    if (!visible() && reason != UpdateReason::Hidden) return;

    w->addToFrame(UpdateRequest::create(reason, this));
}

void View::touchEvent(const TouchEvent *event)
{
    Window *w = window();

    Ref<View> touchTarget;
    if (!parent_ && w->touchTargets_->lookup(event->fingerId(), &touchTarget)) {
        if (touchTarget != this)
            touchTarget->touchEvent(event);
    }
    else {
        for (auto pair: children_) {
            View *child = pair->value();
            if (child->containsGlobal(event->pos())) {
                child->touchEvent(event);
                return;
            }
        }
    }

    mousePos = mapToLocal(event->pos());
    mouseButton = MouseButton::Left;

    if (event->action() == PointerAction::Pressed) {
        w->touchTargets_->establish(event->fingerId(), this);
        pressed();
    }
    else if (event->action() == PointerAction::Released) {
        released();
        if (containsGlobal(event->pos()))
            clicked();
        w->touchTargets_->remove(event->fingerId());
    }
}

void View::mouseEvent(const MouseEvent *event)
{
    Window *w = window();

    if (!parent_ && w->pointerTarget_) {
        if (w->pointerTarget_ != this)
            w->pointerTarget_->mouseEvent(event);
    }
    else {
        for (auto pair: children_) {
            View *child = pair->value();
            if (child->containsGlobal(event->pos())) {
                child->mouseEvent(event);
                return;
            }
        }
    }

    mousePos = mapToLocal(event->pos());
    mouseButton = event->button();

    if (event->action() == PointerAction::Pressed) {
        w->pointerTarget_ = this;
        pressed();
    }
    else if (event->action() == PointerAction::Released) {
        released();
        if (containsGlobal(event->pos()))
            clicked();
        w->pointerTarget_ = 0;
    }
}

void View::mouseWheelEvent(const MouseWheelEvent *event)
{
    for (auto pair: children_) {
        View *child = pair->value();
        if (child->containsGlobal(event->mousePos())) {
            child->mouseWheelEvent(event);
            return;
        }
    }
}

void View::keyEvent(const KeyEvent *event)
{
    key = Key{ event->scanCode(), event->keyCode(), event->modifiers() };

    if (event->action() == KeyAction::Pressed) {
        keyPressed();
    }
    else if (event->action() == KeyAction::Released) {
        keyReleased();
        key = Key{};
    }
}

View *View::fly(View *view)
{
    if (view->parent()) view->parent()->childReady(view);
    return view;
}

void View::init()
{
    if (parent()) parent()->childReady(this);
}

void View::childReady(View *child)
{}

void View::childDone(View *child)
{}

Window *View::window()
{
    if (!window_) {
        if (parent_)
            window_ = parent_->window();
    }
    return window_;
}

Image *View::image()
{
    if (!image_ || image_->size() != size())
        image_ = Image::create(size());
    return image_;
}

uint64_t View::nextSerial() const
{
    return (children_->count() > 0) ? children_->keyAt(children_->count() - 1) + 1 : 1;
}

void View::insertChild(View *child)
{
    child->parent_ = this;
    child->serial_ = nextSerial();
    children_->insert(child->serial_, child);
    childCount += 1;
}

void View::removeChild(View *child)
{
    Ref<View> hook = child;
    children_->remove(child->serial_);
    child->serial_ = 0;
    childCount -= 1;
    childDone(child);
}

void View::adoptChild(View *parent, View *child)
{
    parent->insertChild(child);
}

cairo_surface_t *View::cairoSurface() const
{
    return const_cast<View *>(this)->image()->cairoSurface();
}

}} // namespace cc::ui
