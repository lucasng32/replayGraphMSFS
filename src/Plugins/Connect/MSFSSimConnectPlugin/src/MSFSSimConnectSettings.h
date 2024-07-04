/**
 * Sky Dolly - The Black Sheep for Your Flight Recordings
 *
 * Copyright (c) Oliver Knoll
 * All rights reserved.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation ios (the "Software"), to deal in the Software
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
#ifndef MSFSSIMCONNECTSETTINGS_H
#define MSFSSIMCONNECTSETTINGS_H

#include <memory>

#include <QObject>

#include <Kernel/Settings.h>
#include <PluginManager/Connect/ConnectPluginBaseSettings.h>

struct MSFSSimConnectSettingsPrivate;

class MSFSSimConnectSettings : public ConnectPluginBaseSettings
{
    Q_OBJECT
public:
    MSFSSimConnectSettings() noexcept;
    MSFSSimConnectSettings(const MSFSSimConnectSettings &rhs) = delete;
    MSFSSimConnectSettings(MSFSSimConnectSettings &&rhs) = delete;
    MSFSSimConnectSettings &operator=(const MSFSSimConnectSettings &rhs) = delete;
    MSFSSimConnectSettings &operator=(MSFSSimConnectSettings &&rhs) = delete;
    ~MSFSSimConnectSettings() override;

    /*!
     * The SimConnect local or network connection type.
     *
     * Implementation note: these values are peristed in the application settings.
     */
    enum struct ConnectionType
    {
        First = 0,
        Pipe = First,
        IPv4,
        IPv6,
        Last = IPv6
    };

    ConnectionType getConnectionType() const noexcept;
    void setConnectionType(ConnectionType connectionType) noexcept;

protected:
    void addSettingsExtn(Settings::KeyValues &keyValues) const noexcept override;
    void addKeysWithDefaultsExtn(Settings::KeysWithDefaults &keysWithDefaults) const noexcept override;
    void restoreSettingsExtn(const Settings::ValuesByKey &valuesByKey) noexcept override;
    void restoreDefaultsExtn() noexcept override;

private:
    const std::unique_ptr<MSFSSimConnectSettingsPrivate> d;
};

#endif // MSFSSIMCONNECTSETTINGS_H
