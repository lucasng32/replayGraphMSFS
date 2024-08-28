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

#include <QJsonDocument>
#include <QJsonArray>
#include <QtCore/QDebug>
#include <Kernel/Const.h>
#include <Kernel/Convert.h>
#include <Kernel/Enum.h>
#include <Kernel/Unit.h>
#include <Kernel/SkyMath.h>
#include <Kernel/Version.h>
#include <Kernel/Settings.h>
#include <Model/Logbook.h>
#include <Model/Flight.h>
#include <Model/AircraftType.h>
#include <Model/AircraftInfo.h>
#include <Model/Position.h>
#include <Model/PositionData.h>
#include <Model/Attitude.h>
#include <Model/AttitudeData.h>
#include <Persistence/PersistenceManager.h>
#include <Persistence/Service/FlightService.h>
#include <Persistence/Service/AircraftService.h>
#include <PluginManager/SkyConnectManager.h>
#include <PluginManager/Connect/SkyConnectIntf.h>
#include <PluginManager/Connect/Connect.h>
#include <Widget/Platform.h>
#include <Kernel/SampleRate.h>
#include <PluginManager/Export.h>
#include <Widget/UnitWidgetItem.h>
#include "ServerWidget.h"
#include "ServerSettings.h"
#include "WebSocketServer.h"
#include "src/ui_ServerWidget.h"
#include "ui_ServerWidget.h"

enum struct DataMode{
    Replay,
    Record
};

struct ServerWidgetPrivate
{
    std::unique_ptr<WebSocketServer> server {std::make_unique<WebSocketServer>(1234)};
    bool toReload;
};

// PUBLIC

ServerWidget::ServerWidget(ServerSettings &moduleSettings, QWidget *parent) noexcept
    : QWidget {parent},
      ui {std::make_unique<Ui::ServerWidget>()},
      d {std::make_unique<ServerWidgetPrivate>()}
{
    ui->setupUi(this);
    initUi();
    updateUi();
    frenchConnection();
}

ServerWidget::~ServerWidget() = default;

// PRIVATE
void ServerWidget::initUi() noexcept
{

}

void ServerWidget::frenchConnection() noexcept
{
    auto &skyConnectManager = SkyConnectManager::getInstance();
    connect(&skyConnectManager, &SkyConnectManager::timestampChanged,
            this, &ServerWidget::onTimestampChanged);
    connect(&skyConnectManager, &SkyConnectManager::stateChanged,
            this, &ServerWidget::onStateChanged);
    connect(d->server.get(), &WebSocketServer::messageReceived,
            this, &ServerWidget::onWebMessage);
}

// PRIVATE SLOTS

void ServerWidget::updateUi() noexcept
{

}

void ServerWidget::onStateChanged(Connect::State state) noexcept
{
    d->toReload = true;
}

void ServerWidget::onWebMessage(QString message) noexcept
{
    // Only process slider message if in replay mode
    auto &skyConnectManager = SkyConnectManager::getInstance();

    if (skyConnectManager.isInReplayState()){
        // Try to parse the message as JSON
        QJsonParseError parseError;
        QJsonObject webResponse;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

        if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
            webResponse = jsonDoc.object();
        }
        qDebug() << "json" << webResponse;
        const auto &command = webResponse.value("command");

        if (command == "pause"){
            skyConnectManager.setPaused(SkyConnectIntf::Initiator::App, true);
        }
        if (command == "play"){
            skyConnectManager.setPaused(SkyConnectIntf::Initiator::App, false);
        }
        if (command == "seek"){
            const std::int64_t &sliderVal = webResponse.value("timestamp").toDouble() * 1000;
            skyConnectManager.seek(sliderVal, SkyConnectIntf::SeekMode::Continuous);
        }
    }
}

void ServerWidget::onTimestampChanged(std::int64_t timestamp, TimeVariableData::Access access) noexcept
{
    auto &skyConnectManager = SkyConnectManager::getInstance();
    auto &flight = Logbook::getInstance().getCurrentFlight();
    const auto &aircraft = flight.getUserAircraft();
    const auto &position = aircraft.getPosition();
    const auto &attitude = aircraft.getAttitude();
    const auto currTimestamp = skyConnectManager.getCurrentTimestamp();
    QJsonArray data;
    if (skyConnectManager.isInReplayState()) {
        // get current timestamp of skyConnectManager, send attitude JSON to client
        const auto interpolatedPositionData = Export::resamplePositionDataForExport(aircraft, SampleRate::DefaultResamplingPeriod);
        for (const auto &positionData : interpolatedPositionData) {
            const auto &attitudeData = aircraft.getAttitude().interpolate(positionData.timestamp, TimeVariableData::Access::NoTimeOffset);
            QJsonObject dataStep = {
                {"altitude", positionData.altitude},
                {"longitude", positionData.longitude},
                {"latitude", positionData.latitude},
                {"bank", attitudeData.bank},
                {"pitch", attitudeData.pitch},
                {"heading", attitudeData.trueHeading},
                {"velocityBodyX", attitudeData.velocityBodyX},
                {"velocityBodyY", attitudeData.velocityBodyY},
                {"velocityBodyZ", attitudeData.velocityBodyZ},
                {"velocityWorldX", attitudeData.velocityWorldX},
                {"velocityWorldY", attitudeData.velocityWorldY},
                {"velocityWorldZ", attitudeData.velocityWorldZ},
                {"gForce", attitudeData.gForce},
                {"incidenceAlpha", attitudeData.incidenceAlpha},
                {"timestamp", positionData.timestamp}
            };
            data.append(dataStep);
        }

        QJsonObject dataSend = {
            {"data", data},
            {"timestamp", currTimestamp},
            {"dataMode", static_cast<int>(DataMode::Replay)},
            {"toReload", d->toReload}
        };
        // qDebug() << "sending data for replay" << currTimestamp;
        d->server->sendData(dataSend);
        d->toReload = false;
    }
    else if (skyConnectManager.isInRecordingState() && position.count() > 0){
        const auto &positionData = position.getLast();
        const auto &attitudeData = attitude.getLast();
        if (!positionData.isNull()){
            QJsonObject dataStep = {
                {"altitude", positionData.altitude},
                {"longitude", positionData.longitude},
                {"latitude", positionData.latitude},
                {"bank", attitudeData.bank},
                {"pitch", attitudeData.pitch},
                {"heading", attitudeData.trueHeading},
                {"velocityBodyX", attitudeData.velocityBodyX},
                {"velocityBodyY", attitudeData.velocityBodyY},
                {"velocityBodyZ", attitudeData.velocityBodyZ},
                {"velocityWorldX", attitudeData.velocityWorldX},
                {"velocityWorldY", attitudeData.velocityWorldY},
                {"velocityWorldZ", attitudeData.velocityWorldZ},
                {"gForce", attitudeData.gForce},
                {"incidenceAlpha", attitudeData.incidenceAlpha},
                {"timestamp", positionData.timestamp}
            };
            data.append(dataStep);
            QJsonObject dataSend = {
                {"data", data},
                {"timestamp", currTimestamp},
                {"dataMode", static_cast<int>(DataMode::Record)},
                {"toReload", d->toReload}
            };
            // qDebug() << "sending live data " << currTimestamp;
            d->server->sendData(dataSend);
        }
    }
}

