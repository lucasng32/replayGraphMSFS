/**
 * Sky Dolly - The black sheep for your flight recordings
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

#include <QTimer>
#include <QtGlobal>
#include <QRandomGenerator>

#include "../../Kernel/src/Settings.h"
#include "../../Model/src/World.h"
#include "../../Model/src/Scenario.h"
#include "../../Model/src/AircraftData.h"
#include "AbstractSkyConnect.h"
#include "SkyConnectDummy.h"

namespace {
    // Hz
    constexpr int ReplayRate = 60;
    constexpr int ReplayPeriod = qRound(1000.0 / ReplayRate);
}

class SkyConnectDummyPrivate
{
public:
    SkyConnectDummyPrivate() noexcept
    {
    }

    QTimer replayTimer;
};

// PUBLIC

SkyConnectDummy::SkyConnectDummy(QObject *parent) noexcept
    : AbstractSkyConnect(parent),
      d(std::make_unique<SkyConnectDummyPrivate>())
{
    frenchConnection();
}

SkyConnectDummy::~SkyConnectDummy() noexcept
{
}

// PROTECTED

void SkyConnectDummy::onStartRecording() noexcept
{
}

void SkyConnectDummy::onRecordingPaused(bool paused) noexcept
{
    Q_UNUSED(paused)
}

void SkyConnectDummy::onStopRecording() noexcept
{
}

void SkyConnectDummy::onStartReplay(qint64 currentTimestamp) noexcept {
    Q_UNUSED(currentTimestamp)
    d->replayTimer.start(ReplayPeriod);
}

void SkyConnectDummy::onReplayPaused(bool paused) noexcept
{
    if (paused) {
         d->replayTimer.stop();
    } else {
        d->replayTimer.start(ReplayPeriod);
    }
}

void SkyConnectDummy::onStopReplay() noexcept
{
    d->replayTimer.stop();
}

void SkyConnectDummy::onSeek(qint64 currentTimestamp) noexcept
{
    Q_UNUSED(currentTimestamp)
};

void SkyConnectDummy::onRecordSampleRateChanged(SampleRate::SampleRate sampleRate) noexcept
{
    Q_UNUSED(sampleRate)
}

bool SkyConnectDummy::sendAircraftData(qint64 currentTimestamp) noexcept
{
    Q_UNUSED(currentTimestamp)
    return sendAircraftData();
}

bool SkyConnectDummy::isConnectedWithSim() const noexcept
{
    return true;
}

bool SkyConnectDummy::connectWithSim() noexcept
{
    return true;
}

// PROTECTED SLOTS

void SkyConnectDummy::processEvents() noexcept
{
    updateCurrentTimestamp();
    switch (getState()) {
    case Connect::State::Recording:
        recordData();
        break;
    case Connect::State::Replay:
        replay();
        break;
    default:
        break;
    }
}

// PRIVATE

void SkyConnectDummy::frenchConnection() noexcept
{
    connect(&d->replayTimer, &QTimer::timeout,
            this, &SkyConnectDummy::processEvents);
}

bool SkyConnectDummy::sendAircraftData() noexcept
{
    bool success;

    const AircraftData &currentAircraftData = getCurrentScenario().getUserAircraftConst().interpolateAircraftData(getCurrentTimestamp());
    if (!currentAircraftData.isNull()) {
        // Start the elapsed timer after sending the first sample data
        if (!isElapsedTimerRunning()) {
            startElapsedTimer();
        }
        success = true;
    } else {
        success = false;
    }
    return success;
}

void SkyConnectDummy::recordData() noexcept
{
    AircraftData aircraftData;
    aircraftData.latitude = QRandomGenerator::global()->bounded(180.0);
    aircraftData.longitude = QRandomGenerator::global()->bounded(90.0);
    aircraftData.altitude = QRandomGenerator::global()->bounded(20000.0);
    aircraftData.timestamp = getCurrentTimestamp();
    Aircraft &aircraft = World::getInstance().getCurrentScenario().getUserAircraft();

    aircraft.upsertAircraftData(std::move(aircraftData));
    if (!isElapsedTimerRunning()) {
        // Start the elapsed timer with the arrival of the first sample data
        setCurrentTimestamp(0);
        resetElapsedTime(true);
    }
}

void SkyConnectDummy::replay() noexcept
{
    if (sendAircraftData()) {
        emit currentTimestampChanged(getCurrentTimestamp());
    } else {
        stopReplay();
    }
}
