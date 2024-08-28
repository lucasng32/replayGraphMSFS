/**
 * Sky Dolly - The black sheep for your fposition recordings
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

#include <Kernel/SkyMath.h>
#include "TimeVariableData.h"
#include "SkySearch.h"
#include "AircraftInfo.h"
#include "AttitudeData.h"
#include "Attitude.h"

namespace
{
constexpr double Tension = 0.0;
}

// PUBLIC

Attitude::Attitude(const AircraftInfo &aircraftInfo) noexcept
    : AbstractComponent(aircraftInfo)
{}

const AttitudeData &Attitude::interpolate(std::int64_t timestamp, TimeVariableData::Access access) const noexcept
{
    const AttitudeData *p0 {nullptr}, *p1 {nullptr}, *p2 {nullptr}, *p3 {nullptr};
    const auto timeOffset = access != TimeVariableData::Access::NoTimeOffset ? getAircraftInfo().timeOffset : 0;
    const auto adjustedTimestamp = std::max(timestamp + timeOffset, std::int64_t(0));

    if (getCurrentTimestamp() != adjustedTimestamp || getCurrentAccess() != access) {
        int currentIndex = getCurrentIndex();
        double tn {0.0};
        // Attitude data is always interpolated within an "infinite" interpolation window, in order to
        // take imported "sparse flight plans" into account
        if (SkySearch::getCubicInterpolationSupportData(getData(), adjustedTimestamp, SkySearch::InfinitetInterpolationWindow, currentIndex, &p0, &p1, &p2, &p3)) {
            tn = SkySearch::normaliseTimestamp(*p1, *p2, adjustedTimestamp);
        }
        if (p1 != nullptr) {
            // Aircraft attitude

            // Pitch: [-90, 90] - no discontinuity at +/- 90
            m_currentData.pitch = SkyMath::interpolateHermite(p0->pitch, p1->pitch, p2->pitch, p3->pitch, tn, ::Tension);
            // Bank: [-180, 180] - discontinuity at +/- 180
            m_currentData.bank  = SkyMath::interpolateHermite180(p0->bank, p1->bank, p2->bank, p3->bank, tn, ::Tension);
            // Heading: [0, 360] - discontinuity at 0/360
            m_currentData.trueHeading = SkyMath::interpolateHermite360(p0->trueHeading, p1->trueHeading, p2->trueHeading, p3->trueHeading, tn, ::Tension);

            // Velocity Body
            m_currentData.velocityBodyX = SkyMath::interpolateLinear(p1->velocityBodyX, p2->velocityBodyX, tn);
            m_currentData.velocityBodyY = SkyMath::interpolateLinear(p1->velocityBodyY, p2->velocityBodyY, tn);
            m_currentData.velocityBodyZ = SkyMath::interpolateLinear(p1->velocityBodyZ, p2->velocityBodyZ, tn);

            // Velocity World
            m_currentData.velocityWorldX = SkyMath::interpolateLinear(p1->velocityWorldX, p2->velocityWorldX, tn);
            m_currentData.velocityWorldY = SkyMath::interpolateLinear(p1->velocityWorldY, p2->velocityWorldY, tn);
            m_currentData.velocityWorldZ = SkyMath::interpolateLinear(p1->velocityWorldZ, p2->velocityWorldZ, tn);

            // Acceleration Body
            m_currentData.accelerationBodyX = SkyMath::interpolateLinear(p1->accelerationBodyX, p2->accelerationBodyX, tn);
            m_currentData.accelerationBodyY = SkyMath::interpolateLinear(p1->accelerationBodyY, p2->accelerationBodyY, tn);
            m_currentData.accelerationBodyZ = SkyMath::interpolateLinear(p1->accelerationBodyZ, p2->accelerationBodyZ, tn);

            // Acceleration World
            m_currentData.accelerationWorldX = SkyMath::interpolateLinear(p1->accelerationWorldX, p2->accelerationWorldX, tn);
            m_currentData.accelerationWorldY = SkyMath::interpolateLinear(p1->accelerationWorldY, p2->accelerationWorldY, tn);
            m_currentData.accelerationWorldZ = SkyMath::interpolateLinear(p1->accelerationWorldZ, p2->accelerationWorldZ, tn);

            // Rotation Velocity
            m_currentData.rotationVelocityBodyX = SkyMath::interpolateLinear(p1->rotationVelocityBodyX, p2->rotationVelocityBodyX, tn);
            m_currentData.rotationVelocityBodyY = SkyMath::interpolateLinear(p1->rotationVelocityBodyY, p2->rotationVelocityBodyY, tn);
            m_currentData.rotationVelocityBodyZ = SkyMath::interpolateLinear(p1->rotationVelocityBodyZ, p2->rotationVelocityBodyZ, tn);

            // G Force and AoA
            m_currentData.gForce = SkyMath::interpolateLinear(p1->gForce, p2->gForce, tn);
            m_currentData.incidenceAlpha = SkyMath::interpolateLinear(p1->incidenceAlpha, p2->incidenceAlpha, tn);

            // On ground (boolean value - no interpolation)
            m_currentData.onGround = p1->onGround;

            m_currentData.timestamp = adjustedTimestamp;
        } else {
            // No recorded data, or the timestamp exceeds the timestamp of the last recorded data
            m_currentData.reset();
        }

        setCurrentIndex(currentIndex);
        setCurrentTimestamp(adjustedTimestamp);
        setCurrentAccess(access);
    }
    return m_currentData;
}

template class AbstractComponent<AttitudeData>;
