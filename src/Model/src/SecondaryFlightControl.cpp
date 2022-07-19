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
#include <algorithm>
#include <cstdint>

#ifdef DEBUG
#include <QDebug>
#endif

#include <Kernel/Settings.h>
#include <Kernel/SkyMath.h>
#include "TimeVariableData.h"
#include "SkySearch.h"
#include "AircraftInfo.h"
#include "SecondaryFlightControlData.h"
#include "SecondaryFlightControl.h"

namespace
{
    constexpr double Tension = 0.0;
}

// PUBLIC

SecondaryFlightControl::SecondaryFlightControl(const AircraftInfo &aircraftInfo) noexcept
    : AbstractComponent(aircraftInfo)
{
#ifdef DEBUG
    qDebug() << "SecondaryFlightControl::SecondaryFlightControl: CREATED";
#endif
}

SecondaryFlightControl::~SecondaryFlightControl() noexcept
{
#ifdef DEBUG
    qDebug() << "SecondaryFlightControl::SecondaryFlightControl: DELETED";
#endif
}

const SecondaryFlightControlData &SecondaryFlightControl::interpolate(std::int64_t timestamp, TimeVariableData::Access access) noexcept
{
    const SecondaryFlightControlData *p1 {nullptr}, *p2 {nullptr};
    const std::int64_t timeOffset = access != TimeVariableData::Access::Export ? getAircraftInfo().timeOffset : 0;
    const std::int64_t adjustedTimestamp = std::max(timestamp + timeOffset, std::int64_t(0));

    if (getCurrentTimestamp() != adjustedTimestamp || getCurrentAccess() != access) {

        int currentIndex = getCurrentIndex();
        double tn {0.0};
        switch (access) {
        case TimeVariableData::Access::Linear:
            [[fallthrough]];
        case TimeVariableData::Access::Export:
            if (SkySearch::getLinearInterpolationSupportData(getData(), adjustedTimestamp, SkySearch::DefaultInterpolationWindow, currentIndex, &p1, &p2)) {
                tn = SkySearch::normaliseTimestamp(*p1, *p2, adjustedTimestamp);
            }
            break;
        case TimeVariableData::Access::Seek:
            // Get the last sample data just before the seeked position
            // (that sample point may lie far outside of the "sample window")
            currentIndex = SkySearch::updateStartIndex(getData(), currentIndex, adjustedTimestamp);
            if (currentIndex != SkySearch::InvalidIndex) {
                p1 = &getData().at(currentIndex);
                p2 = p1;
                tn = 0.0;
            } else {
                p1 = p2 = nullptr;
            }
            break;
        }

        if (p1 != nullptr) {
            m_currentSecondaryFlightControlData.leadingEdgeFlapsLeftPosition = SkyMath::interpolateLinear(p1->leadingEdgeFlapsLeftPosition, p2->leadingEdgeFlapsLeftPosition, tn);
            m_currentSecondaryFlightControlData.leadingEdgeFlapsRightPosition = SkyMath::interpolateLinear(p1->leadingEdgeFlapsRightPosition, p2->leadingEdgeFlapsRightPosition, tn);
            m_currentSecondaryFlightControlData.trailingEdgeFlapsLeftPosition = SkyMath::interpolateLinear(p1->trailingEdgeFlapsLeftPosition, p2->trailingEdgeFlapsLeftPosition, tn);
            m_currentSecondaryFlightControlData.trailingEdgeFlapsRightPosition = SkyMath::interpolateLinear(p1->trailingEdgeFlapsRightPosition, p2->trailingEdgeFlapsRightPosition, tn);
            m_currentSecondaryFlightControlData.spoilersHandlePosition = SkyMath::interpolateLinear(p1->spoilersHandlePosition, p2->spoilersHandlePosition, tn);

            // No interpolation for flaps handle position
            m_currentSecondaryFlightControlData.flapsHandleIndex = p1->flapsHandleIndex;
            // Certain aircraft automatically override the FLAPS HANDLE INDEX, so values need to be repeatedly set
            if (Settings::getInstance().isRepeatFlapsHandleIndexEnabled()) {
                // We do that my storing the previous values (when the flaps are set)...
                m_previousSecondaryFlightControlData = m_currentSecondaryFlightControlData;
            } else {
                // "Repeat values" setting disabled
                m_previousSecondaryFlightControlData = SecondaryFlightControlData::NullData;
            }

            m_currentSecondaryFlightControlData.timestamp = adjustedTimestamp;
        } else if (!m_previousSecondaryFlightControlData.isNull()) {
            // ... and send the previous values again (for as long as the flaps are extracted)
            m_currentSecondaryFlightControlData = m_previousSecondaryFlightControlData;
            m_currentSecondaryFlightControlData.timestamp = adjustedTimestamp;
        } else {
            // No recorded data, or the timestamp exceeds the timestamp of the last recorded position
            m_currentSecondaryFlightControlData = SecondaryFlightControlData::NullData;
        }

        setCurrentIndex(currentIndex);
        setCurrentTimestamp(adjustedTimestamp);
        setCurrentAccess(access);
    }
    return m_currentSecondaryFlightControlData;
}

template class AbstractComponent<SecondaryFlightControlData>;
