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
#include "Model/PrimaryFlightControl.h"
#include <Persistence/FlightSelector.h>
#include <Persistence/Service/LogbookService.h>

namespace Client {
enum Event {
    LoadFlight,
    ReplaySeek,
    ReplayPlay,
    ReplayPause,
    ReplayStop,
    RecordingStart,
    RecordingPause,
    RecordingPlay,
    RecordingStop
};
}
namespace Server{
enum ServerEvent {
    Logbook = 9,
    FlightLoaded,
    ReplayTimestampChanged,
    ReplayPlay,
    ReplayPause,
    PrevRecordingData,
    RecordingTimestampChanged,
    RecordingStart,
    RecordingPause,
    RecordingStop
};
}

struct ServerWidgetPrivate
{
    std::unique_ptr<WebSocketServer> server {std::make_unique<WebSocketServer>(1234)};
    std::unique_ptr<LogbookService> logbookService {std::make_unique<LogbookService>()};
    std::unique_ptr<FlightService> flightService {std::make_unique<FlightService>()};
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

void ServerWidget::loadFlight(std::int64_t selectedFlightId) noexcept
{
    qDebug() << "load flight id " << selectedFlightId;
    const bool ok = d->flightService->restoreFlight(selectedFlightId, Logbook::getInstance().getCurrentFlight());
    if (!ok) {}
}

void ServerWidget::sendLogbookData() noexcept
{
    if (PersistenceManager::getInstance().isConnected()) {
        FlightSelector flightSelector;
        const auto &flight = Logbook::getInstance().getCurrentFlight();
        auto summaries = d->logbookService->getFlightSummaries(flightSelector);
        QJsonArray summariesJson;
        for (auto summary: summaries){
            QJsonObject summaryJson = {
                {"flightId", summary.flightId},
                {"title", summary.title},
                {"aircraftType", summary.aircraftType},
                {"aircraftCount", static_cast<int>(summary.aircraftCount)},
                {"flightNumber", summary.flightNumber},
                {"creationDate", summary.creationDate.date().toString("yyyy.MM.dd")},
                {"startTime", summary.startSimulationLocalTime.time().toString("hh:mm:ss")},
                {"startLocation", summary.startLocation},
                {"endTime", summary.endSimulationLocalTime.time().toString("hh:mm:ss")},
                {"endLocation", summary.endLocation},
                {"duration", summary.startSimulationLocalTime.msecsTo(summary.endSimulationLocalTime)}
            };
            summariesJson.push_back(summaryJson);
        }
        QJsonObject data = {
            {"logbook", summariesJson}
        };
        d->server->sendData(Server::Logbook, data);
    }
}

void ServerWidget::sendReplayData() noexcept
{
    auto &skyConnectManager = SkyConnectManager::getInstance();
    auto &flight = Logbook::getInstance().getCurrentFlight();
    const auto &aircraft = flight.getUserAircraft();
    const auto &aircraftInfo = aircraft.getAircraftInfo();
    const auto &aircraftType = aircraftInfo.aircraftType;
    const auto &position = aircraft.getPosition();
    const auto &attitude = aircraft.getAttitude();
    const auto currTimestamp = skyConnectManager.getCurrentTimestamp();
    const auto &primaryFlightControl = aircraft.getPrimaryFlightControl();
    QJsonObject aircraftModel = {
        {"flapsUpStallSpeed", aircraftType.flapsUpStallSpeed},
        {"flapsFullStallSpeed", aircraftType.flapsFullStallSpeed},
        {"stallAlpha", aircraftType.stallAlpha},
        {"staticPitch", aircraftType.staticPitch},
        {"zeroLiftAlpha", aircraftType.zeroLiftAlpha}
    };
    QJsonArray data;
    if (skyConnectManager.isInReplayState()) {
        // get current timestamp of skyConnectManager, send attitude JSON to client
        const auto interpolatedPositionData = Export::resamplePositionDataForExport(aircraft, SampleRate::DefaultResamplingPeriod);
        for (const auto &positionData : interpolatedPositionData) {
            const auto &attitudeData = attitude.interpolate(positionData.timestamp, TimeVariableData::Access::NoTimeOffset);
            const auto &primaryFlightControlData = primaryFlightControl.interpolate(positionData.timestamp, TimeVariableData::Access::NoTimeOffset);
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
                {"indicatedAirspeed", attitudeData.indicatedAirspeed},
                {"trueAirspeed", attitudeData.trueAirspeed},
                {"groundSpeed", attitudeData.groundSpeed},
                {"windSpeed", attitudeData.windSpeed},
                {"windDirection", attitudeData.windDirection},
                {"aileronPosition", primaryFlightControlData.aileronPosition},
                {"elevatorPosition", primaryFlightControlData.elevatorPosition},
                {"rudderPosition", primaryFlightControlData.rudderPosition},
                {"timestamp", positionData.timestamp}
            };
            data.append(dataStep);
        }

        QJsonObject dataSend = {
            {"flightData", data},
            {"timestamp", currTimestamp},
            {"aircraftModel", aircraftModel}
        };
        // qDebug() << "sending data for replay" << currTimestamp;
        d->server->sendData(Server::FlightLoaded, dataSend);
    }
}

void ServerWidget::onReplayTimestampChanged(std::int64_t timestamp) noexcept
{
    QJsonObject data = {
        {"timestamp", timestamp}
    };
    d->server->sendData(Server::ReplayTimestampChanged, data);
}

void ServerWidget::sendPrevRecordingData() noexcept
{
    auto &skyConnectManager = SkyConnectManager::getInstance();
    auto &flight = Logbook::getInstance().getCurrentFlight();
    const auto &aircraft = flight.getUserAircraft();
    const auto &aircraftInfo = aircraft.getAircraftInfo();
    const auto &aircraftType = aircraftInfo.aircraftType;
    const auto &position = aircraft.getPosition();
    const auto &attitude = aircraft.getAttitude();
    const auto currTimestamp = skyConnectManager.getCurrentTimestamp();
    const auto &primaryFlightControl = aircraft.getPrimaryFlightControl();
    QJsonObject aircraftModel = {
        {"flapsUpStallSpeed", aircraftType.flapsUpStallSpeed},
        {"flapsFullStallSpeed", aircraftType.flapsFullStallSpeed},
        {"stallAlpha", aircraftType.stallAlpha},
        {"staticPitch", aircraftType.staticPitch},
        {"zeroLiftAlpha", aircraftType.zeroLiftAlpha}
    };
    QJsonArray data;
    if (skyConnectManager.isInRecordingState() && position.count() > 0){
        const auto interpolatedPositionData = Export::resamplePositionDataForExport(aircraft, SampleRate::DefaultResamplingPeriod);
        for (const auto &positionData : interpolatedPositionData) {
            const auto &attitudeData = attitude.interpolate(positionData.timestamp, TimeVariableData::Access::NoTimeOffset);
            const auto &primaryFlightControlData = primaryFlightControl.interpolate(positionData.timestamp, TimeVariableData::Access::NoTimeOffset);
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
                {"indicatedAirspeed", attitudeData.indicatedAirspeed},
                {"trueAirspeed", attitudeData.trueAirspeed},
                {"groundSpeed", attitudeData.groundSpeed},
                {"windSpeed", attitudeData.windSpeed},
                {"windDirection", attitudeData.windDirection},
                {"aileronPosition", primaryFlightControlData.aileronPosition},
                {"elevatorPosition", primaryFlightControlData.elevatorPosition},
                {"rudderPosition", primaryFlightControlData.rudderPosition},
                {"timestamp", positionData.timestamp}
            };
            data.append(dataStep);
        }

        QJsonObject dataSend = {
            {"data", data},
            {"timestamp", currTimestamp},
            {"aircraftModel", aircraftModel}
        };
        d->server->sendData(Server::PrevRecordingData, dataSend);
    }
}

void ServerWidget::onRecordingTimestampChanged(std::int64_t timestamp) noexcept
{
    auto &skyConnectManager = SkyConnectManager::getInstance();
    auto &flight = Logbook::getInstance().getCurrentFlight();
    const auto &aircraft = flight.getUserAircraft();
    const auto &aircraftInfo = aircraft.getAircraftInfo();
    const auto &position = aircraft.getPosition();
    const auto &attitude = aircraft.getAttitude();
    const auto &primaryFlightControl = aircraft.getPrimaryFlightControl();
    if (skyConnectManager.isInRecordingState() && position.count() > 0){
        const auto &positionData = position.getLast();
        const auto &attitudeData = attitude.getLast();
        const auto &primaryFlightControlData = primaryFlightControl.getLast();
        if (!positionData.isNull()){
            QJsonObject flightData = {
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
                {"indicatedAirspeed", attitudeData.indicatedAirspeed},
                {"trueAirspeed", attitudeData.trueAirspeed},
                {"groundSpeed", attitudeData.groundSpeed},
                {"windSpeed", attitudeData.windSpeed},
                {"windDirection", attitudeData.windDirection},
                {"aileronPosition", primaryFlightControlData.aileronPosition},
                {"elevatorPosition", primaryFlightControlData.elevatorPosition},
                {"rudderPosition", primaryFlightControlData.rudderPosition},
                {"timestamp", positionData.timestamp}
            };
            QJsonObject dataSend = {
                {"flightData", flightData},
                {"timestamp", timestamp}
            };
            d->server->sendData(Server::RecordingTimestampChanged, dataSend);
        }
    }
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
    connect(d->server.get(), &WebSocketServer::newClientConnect,
            this, &ServerWidget::onNewClientConnect);
    connect(&skyConnectManager, &SkyConnectManager::recordingStopped,
            this, &ServerWidget::onRecordingStopped);
}

// PRIVATE SLOTS

void ServerWidget::updateUi() noexcept
{

}

void ServerWidget::onStateChanged(Connect::State state) noexcept
{
    switch(state){
        case Connect::State::Disconnected:
            break;
        case Connect::State::Connected:
            sendReplayData();
            sendPrevRecordingData();
            break;
        case Connect::State::Recording:
            d->server->sendData(Server::RecordingStart, QJsonObject {});
            break;
        case Connect::State::RecordingPaused:
            d->server->sendData(Server::RecordingPause, QJsonObject {});
            break;
        case Connect::State::Replay:
            d->server->sendData(Server::ReplayPlay, QJsonObject {});
            break;
        case Connect::State::ReplayPaused:
            d->server->sendData(Server::ReplayPause, QJsonObject {});
            break;
    }
}

void ServerWidget::onRecordingStopped() noexcept
{
    d->server->sendData(Server::RecordingStop, QJsonObject {});
    sendLogbookData();
}

void ServerWidget::onWebMessage(QString message) noexcept
{
    auto &skyConnectManager = SkyConnectManager::getInstance();

    // Try to parse the message as JSON
    QJsonParseError parseError;
    QJsonObject webResponse;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
        webResponse = jsonDoc.object();
    }
    const int &event = webResponse.value("event").toVariant().toInt();
    const QJsonObject &data = webResponse.value("data").toObject();

    if (skyConnectManager.isConnected()){
        switch(event){
            case Client::LoadFlight:
                qDebug() << "loaded received";
                loadFlight(data.value("flightId").toVariant().toInt());
                sendReplayData();
                break;
            case Client::ReplaySeek:
                skyConnectManager.seek(data.value("timestamp").toDouble() * 1000, SkyConnectIntf::SeekMode::Continuous);
                break;
            case Client::ReplayPlay:
                skyConnectManager.setPaused(SkyConnectIntf::Initiator::App, false);
                break;
            case Client::ReplayPause:
                skyConnectManager.setPaused(SkyConnectIntf::Initiator::App, true);
                break;
            case Client::ReplayStop:
                skyConnectManager.stopReplay();
                break;
            case Client::RecordingStart:
                skyConnectManager.startRecording(SkyConnectIntf::RecordingMode::SingleAircraft);
                break;
            case Client::RecordingPause:
                skyConnectManager.setPaused(SkyConnectIntf::Initiator::App, true);
                break;
            case Client::RecordingPlay:
                skyConnectManager.setPaused(SkyConnectIntf::Initiator::App, false);
                break;
            case Client::RecordingStop:
                skyConnectManager.stopRecording();
                break;
        }
    }
}

void ServerWidget::onNewClientConnect() noexcept
{
    sendLogbookData();
    qDebug() << "send logbook";
    if (SkyConnectManager::getInstance().isConnected()){
        sendReplayData();
        sendPrevRecordingData();
    }
}



void ServerWidget::onTimestampChanged(std::int64_t timestamp, TimeVariableData::Access access) noexcept
{
    auto &skyConnectManager = SkyConnectManager::getInstance();
    auto &flight = Logbook::getInstance().getCurrentFlight();
    const auto &aircraft = flight.getUserAircraft();
    const auto &position = aircraft.getPosition();
    if (skyConnectManager.isInReplayState()) {
        onReplayTimestampChanged(timestamp);
    }
    else if (skyConnectManager.isInRecordingState() && position.count() > 0){
        onRecordingTimestampChanged(timestamp);
    }
}

