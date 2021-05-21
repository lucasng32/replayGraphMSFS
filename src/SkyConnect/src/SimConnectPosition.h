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
#ifndef SIMCONNECTPOSITION_H
#define SIMCONNECTPOSITION_H

#include <windows.h>

#include "../../Kernel/src/SkyMath.h"
#include "../../Model/src/SimType.h"
#include "../../Model/src/PositionData.h"

/*!
 * Simulation variables which represent the aircraft's position, attitude and velocities.
 *
 * Implementation note: this struct needs to be packed.
 */
#pragma pack(push, 1)
struct SimConnectPosition
{
    // Aircraft position
    double latitude;
    double longitude;
    double altitude;
    double pitch;
    double bank;
    double heading;

    // Velocity
    double velocityBodyX;
    double velocityBodyY;
    double velocityBodyZ;
    double rotationVelocityBodyX;
    double rotationVelocityBodyY;
    double rotationVelocityBodyZ;

    inline PositionData toPositionData() const noexcept
    {
        PositionData positionData;
        positionData.latitude = latitude;
        positionData.longitude = longitude;
        positionData.altitude = altitude;
        positionData.pitch = pitch;
        positionData.bank = bank;
        positionData.heading = heading;

        positionData.velocityBodyX = velocityBodyX;
        positionData.velocityBodyY = velocityBodyY;
        positionData.velocityBodyZ = velocityBodyZ;
        positionData.rotationVelocityBodyX = rotationVelocityBodyX;
        positionData.rotationVelocityBodyY = rotationVelocityBodyY;
        positionData.rotationVelocityBodyZ = rotationVelocityBodyZ;

        return positionData;
    }

    inline void fromPositionData(const PositionData &positionData) noexcept
    {
        latitude = positionData.latitude;
        longitude = positionData.longitude;
        altitude = positionData.altitude;
        pitch = positionData.pitch;
        bank = positionData.bank;
        heading = positionData.heading;

        velocityBodyX = positionData.velocityBodyX;
        velocityBodyY = positionData.velocityBodyY;
        velocityBodyZ = positionData.velocityBodyZ;
        rotationVelocityBodyX = positionData.rotationVelocityBodyX;
        rotationVelocityBodyY = positionData.rotationVelocityBodyY;
        rotationVelocityBodyZ = positionData.rotationVelocityBodyZ;
    }

    static void addToDataDefinition(HANDLE simConnectHandle) noexcept;
};
#pragma pack(pop)

#endif // SIMCONNECTPOSITION_H
