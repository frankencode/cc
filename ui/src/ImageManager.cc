/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/ImageManager>
#include <cc/Singleton>
#include <cc/ValueSource>

namespace cc {
namespace ui {

ImageManager *ImageManager::instance()
{
    return Singleton<ImageManager>::instance();
}

ImageManager::ImageManager()
{}

void ImageManager::registerPlugin(ImagePlugin *plugin)
{
    plugins_->establish(plugin->name(), plugin);
}

bool ImageManager::getPlugin(const String &name, const ImagePlugin **plugin) const
{
    return plugins_->lookup(name, plugin);
}

Ref< Source<const ImagePlugin *> > ImageManager::getAllPlugins() const
{
    return ValueSource<Plugins::Instance, const ImagePlugin *>::open(plugins_);
}

Image ImageManager::load(const String &uri, unsigned index) const
{
    int j = uri->scan(':');
    if (j == 0) return Image{};
    String scheme = uri->select(j);
    String path;
    if (j < uri->count()) path = uri->select(j + 1, uri->count());
    const ImagePlugin *plugin = 0;
    if (!getPlugin(scheme, &plugin)) return Image{};
    return plugin->load(path, index);
}

}} // namespace cc::ui
