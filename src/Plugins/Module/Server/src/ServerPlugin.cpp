/**
 * Sky Dolly - The Black Sheep for Your Flight Recordings
 *
 * Copyright (c) Oliver Knoll
 * All rights reserved.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <memory>

#include <QObject>
#include <QCoreApplication>

#include <PluginManager/Module/ModuleBaseSettings.h>
#include "ServerWidget.h"
#include "ServerSettings.h"
#include "ServerPlugin.h"

namespace {
    constexpr const char *ServerModuleUuid {"2c77a56a-2c20-49f7-83ea-55da2dfd22f0"};
}

struct ServerPluginPrivate
{
    ServerSettings moduleSettings;
    std::unique_ptr<ServerWidget> serverWidget {std::make_unique<ServerWidget>(moduleSettings)};
};

// PUBLIC

ServerPlugin::ServerPlugin(QObject *parent) noexcept
    : AbstractModule {parent},
      d {std::make_unique<ServerPluginPrivate>()}
{
    restoreSettings(QUuid(::ServerModuleUuid));
}

ServerPlugin::~ServerPlugin()
{
    storeSettings(QUuid(::ServerModuleUuid));
};

QUuid ServerPlugin::getUuid() const noexcept
{
    static const QUuid uuid {::ServerModuleUuid};
    return uuid;
}

QString ServerPlugin::getModuleName() const noexcept
{
    return QCoreApplication::translate("ServerPlugin", "Server");
}

QWidget *ServerPlugin::getWidget() const noexcept
{
    return d->serverWidget.get();
}

// PROTECTED

ModuleBaseSettings &ServerPlugin::getModuleSettings() const noexcept
{
    return d->moduleSettings;
}


