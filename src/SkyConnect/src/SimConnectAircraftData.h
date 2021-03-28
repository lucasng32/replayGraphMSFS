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
#ifndef SIMCONNECTAIRCRAFTDATA_H
#define SIMCONNECTAIRCRAFTDATA_H

#include <windows.h>

#include "../../Kernel/src/SimTypes.h"
#include "../../Kernel/src/SkyMath.h"
#include "../../Kernel/src/AircraftData.h"

/*!
 * Simulation variables which represent the aircraft's position, attitude and velocities.
 *
 * Implementation note: this struct needs to be packed.
 */
#pragma pack(push, 1)
struct SimConnectAircraftData
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

    // Aircraft controls
    double yokeXPosition;
    double yokeYPosition;
    double rudderPosition;
    double elevatorPosition;
    double aileronPosition;    

    // General engine
    double throttleLeverPosition1;
    double throttleLeverPosition2;
    double throttleLeverPosition3;
    double throttleLeverPosition4;
    double propellerLeverPosition1;
    double propellerLeverPosition2;
    double propellerLeverPosition3;
    double propellerLeverPosition4;
    double mixtureLeverPosition1;
    double mixtureLeverPosition2;
    double mixtureLeverPosition3;
    double mixtureLeverPosition4;

    // Flaps & speed brakes
    double leadingEdgeFlapsLeftPercent;
    double leadingEdgeFlapsRightPercent;
    double trailingEdgeFlapsLeftPercent;
    double trailingEdgeFlapsRightPercent;
    double spoilersHandlePosition;
    qint32 flapsHandleIndex;

    // Gear, brakes & handles
    qint32 gearHandlePosition;
    double brakeLeftPosition;
    double brakeRightPosition;
    double waterRudderHandlePosition;
    double tailhookPosition;
    double canopyOpen;

    // Lights
    qint32 lightStates;

    inline AircraftData toAircraftData() const {
        AircraftData aircraftData;

        aircraftData.latitude = latitude;
        aircraftData.longitude = longitude;
        aircraftData.altitude = altitude;
        aircraftData.pitch = pitch;
        aircraftData.bank = bank;
        aircraftData.heading = heading;

        aircraftData.velocityBodyX = velocityBodyX;
        aircraftData.velocityBodyY = velocityBodyY;
        aircraftData.velocityBodyZ = velocityBodyZ;
        aircraftData.rotationVelocityBodyX = rotationVelocityBodyX;
        aircraftData.rotationVelocityBodyY = rotationVelocityBodyY;
        aircraftData.rotationVelocityBodyZ = rotationVelocityBodyZ;

        aircraftData.yokeXPosition = SkyMath::fromPosition(yokeXPosition);
        aircraftData.yokeYPosition = SkyMath::fromPosition(yokeYPosition);
        aircraftData.rudderPosition = SkyMath::fromPosition(rudderPosition);
        aircraftData.elevatorPosition = SkyMath::fromPosition(elevatorPosition);
        aircraftData.aileronPosition = SkyMath::fromPosition(aileronPosition);

        // Note: the throttle can also yield negative thrust, hence the Sky Dolly internal type
        //       position (qint16) which supports negative values as well
        aircraftData.throttleLeverPosition1 = SkyMath::fromPosition(throttleLeverPosition1);
        aircraftData.throttleLeverPosition2 = SkyMath::fromPosition(throttleLeverPosition2);
        aircraftData.throttleLeverPosition3 = SkyMath::fromPosition(throttleLeverPosition3);
        aircraftData.throttleLeverPosition4 = SkyMath::fromPosition(throttleLeverPosition4);
        aircraftData.propellerLeverPosition1 = SkyMath::fromPosition(propellerLeverPosition1);
        aircraftData.propellerLeverPosition2 = SkyMath::fromPosition(propellerLeverPosition2);
        aircraftData.propellerLeverPosition3 = SkyMath::fromPosition(propellerLeverPosition3);
        aircraftData.propellerLeverPosition4 = SkyMath::fromPosition(propellerLeverPosition4);
        aircraftData.mixtureLeverPosition1 = SkyMath::fromPercent(mixtureLeverPosition1);
        aircraftData.mixtureLeverPosition2 = SkyMath::fromPercent(mixtureLeverPosition2);
        aircraftData.mixtureLeverPosition3 = SkyMath::fromPercent(mixtureLeverPosition3);
        aircraftData.mixtureLeverPosition4 = SkyMath::fromPercent(mixtureLeverPosition4);

        aircraftData.leadingEdgeFlapsLeftPercent = SkyMath::fromPercent(leadingEdgeFlapsLeftPercent);
        aircraftData.leadingEdgeFlapsRightPercent = SkyMath::fromPercent(leadingEdgeFlapsRightPercent);
        aircraftData.trailingEdgeFlapsLeftPercent = SkyMath::fromPercent(trailingEdgeFlapsLeftPercent);
        aircraftData.trailingEdgeFlapsRightPercent = SkyMath::fromPercent(trailingEdgeFlapsRightPercent);
        aircraftData.spoilersHandlePosition = SkyMath::fromPercent(spoilersHandlePosition);
        aircraftData.flapsHandleIndex = flapsHandleIndex;

        aircraftData.gearHandlePosition = gearHandlePosition != 0;
        aircraftData.brakeLeftPosition= SkyMath::fromPosition(brakeLeftPosition);
        aircraftData.brakeRightPosition = SkyMath::fromPosition(brakeRightPosition);
        aircraftData.waterRudderHandlePosition = SkyMath::fromPosition(waterRudderHandlePosition);
        aircraftData.tailhookPosition = SkyMath::fromPercent(tailhookPosition);
        aircraftData.canopyOpen = SkyMath::fromPercent(canopyOpen);

        aircraftData.lightStates = SimTypes::LightStates(lightStates);

        return aircraftData;
    }

    inline void fromAircraftData(const AircraftData &aircraftData) {
        latitude = aircraftData.latitude;
        longitude = aircraftData.longitude;
        altitude = aircraftData.altitude;
        pitch = aircraftData.pitch;
        bank = aircraftData.bank;
        heading = aircraftData.heading;

        velocityBodyX = aircraftData.velocityBodyX;
        velocityBodyY = aircraftData.velocityBodyY;
        velocityBodyZ = aircraftData.velocityBodyZ;
        rotationVelocityBodyX = aircraftData.rotationVelocityBodyX;
        rotationVelocityBodyY = aircraftData.rotationVelocityBodyY;
        rotationVelocityBodyZ = aircraftData.rotationVelocityBodyZ;

        yokeXPosition = SkyMath::toPosition(aircraftData.yokeXPosition);
        yokeYPosition = SkyMath::toPosition(aircraftData.yokeYPosition);
        rudderPosition = SkyMath::toPosition(aircraftData.rudderPosition);
        elevatorPosition = SkyMath::toPosition(aircraftData.elevatorPosition);
        aileronPosition = SkyMath::toPosition(aircraftData.aileronPosition);

        throttleLeverPosition1 = SkyMath::toPosition(aircraftData.throttleLeverPosition1);
        throttleLeverPosition2 = SkyMath::toPosition(aircraftData.throttleLeverPosition2);
        throttleLeverPosition3 = SkyMath::toPosition(aircraftData.throttleLeverPosition3);
        throttleLeverPosition4 = SkyMath::toPosition(aircraftData.throttleLeverPosition4);
        propellerLeverPosition1 = SkyMath::toPosition(aircraftData.propellerLeverPosition1);
        propellerLeverPosition2 = SkyMath::toPosition(aircraftData.propellerLeverPosition2);
        propellerLeverPosition3 = SkyMath::toPosition(aircraftData.propellerLeverPosition3);
        propellerLeverPosition4 = SkyMath::toPosition(aircraftData.propellerLeverPosition4);
        mixtureLeverPosition1 = SkyMath::toPercent(aircraftData.mixtureLeverPosition1);
        mixtureLeverPosition2 = SkyMath::toPercent(aircraftData.mixtureLeverPosition2);
        mixtureLeverPosition3 = SkyMath::toPercent(aircraftData.mixtureLeverPosition3);
        mixtureLeverPosition4 = SkyMath::toPercent(aircraftData.mixtureLeverPosition4);

        leadingEdgeFlapsLeftPercent = SkyMath::toPercent(aircraftData.leadingEdgeFlapsLeftPercent);
        leadingEdgeFlapsRightPercent = SkyMath::toPercent(aircraftData.leadingEdgeFlapsRightPercent);
        trailingEdgeFlapsLeftPercent = SkyMath::toPercent(aircraftData.trailingEdgeFlapsLeftPercent);
        trailingEdgeFlapsRightPercent = SkyMath::toPercent(aircraftData.trailingEdgeFlapsRightPercent);
        spoilersHandlePosition = SkyMath::toPercent(aircraftData.spoilersHandlePosition);
        flapsHandleIndex = aircraftData.flapsHandleIndex;

        gearHandlePosition = aircraftData.gearHandlePosition ? 1 : 0;
        brakeLeftPosition = SkyMath::toPosition(aircraftData.brakeLeftPosition);
        brakeRightPosition = SkyMath::toPosition(aircraftData.brakeRightPosition);
        waterRudderHandlePosition = SkyMath::toPosition(aircraftData.waterRudderHandlePosition);
        tailhookPosition = SkyMath::toPercent(aircraftData.tailhookPosition);
        canopyOpen = SkyMath::toPercent(aircraftData.canopyOpen);

        lightStates = aircraftData.lightStates;
    }

    static void addToDataDefinition(HANDLE simConnectHandle);
};
#pragma pack(pop)

#endif // SIMCONNECTAIRCRAFTDATA_H
