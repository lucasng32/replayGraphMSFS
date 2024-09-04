/**
 * Sky Dolly - The Black Sheep for your Flight Recordings
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
#ifndef SIMCONNECTATTITUDEINFOH
#define SIMCONNECTATTITUDEINFOH

#include <cstdint>

#include <windows.h>
#include <SimConnect.h>

#include <Model/SimVar.h>
#include <Model/AttitudeData.h>

/*!
 * Aircraft attitude simulation variables that are either stored for information purposes only
 * or that are sent exclusively to the user aircraft as events.
 *
 * Implementation note: this struct needs to be packed.
 */
#pragma pack(push, 1)
struct SimConnectAttitudeInfo
{
    std::int32_t onGround {0};

    // Velocity World
    double velocityWorldX {0.0};
    double velocityWorldY {0.0};
    double velocityWorldZ {0.0};

    // Acceleration Body
    double accelerationBodyX {0.0};
    double accelerationBodyY {0.0};
    double accelerationBodyZ {0.0};

    // Acceleration World
    double accelerationWorldX {0.0};
    double accelerationWorldY {0.0};
    double accelerationWorldZ {0.0};

    // Rotation Velocity
    double rotationVelocityBodyX {0.0};
    double rotationVelocityBodyY {0.0};
    double rotationVelocityBodyZ {0.0};

    // G Force
    double gForce {0.0};

    // AoA
    double incidenceAlpha {0.0};

    // Airspeed and Ground speed
    double indicatedAirspeed {0.0};
    double trueAirspeed {0.0};
    double groundSpeed {0.0};

    // Wind
    double windSpeed {0.0};
    double windDirection {0.0};

    SimConnectAttitudeInfo(const AttitudeData &data) noexcept
        : SimConnectAttitudeInfo()
    {
        fromAttitudeData(data);
    }

    SimConnectAttitudeInfo() = default;

    inline void fromAttitudeData(const AttitudeData &data) noexcept
    {
        onGround = data.onGround ? 1 : 0;

        velocityWorldX = data.velocityWorldX;
        velocityWorldY = data.velocityWorldY;
        velocityWorldZ = data.velocityWorldZ;

        accelerationBodyX = data.accelerationBodyX;
        accelerationBodyY = data.accelerationBodyY;
        accelerationBodyZ = data.accelerationBodyZ;

        accelerationWorldX = data.accelerationWorldX;
        accelerationWorldY = data.accelerationWorldY;
        accelerationWorldZ = data.accelerationWorldZ;

        rotationVelocityBodyX = data.rotationVelocityBodyX;
        rotationVelocityBodyY = data.rotationVelocityBodyY;
        rotationVelocityBodyZ = data.rotationVelocityBodyZ;

        gForce = data.gForce;

        incidenceAlpha = data.incidenceAlpha;

        indicatedAirspeed = data.indicatedAirspeed;
        trueAirspeed = data.trueAirspeed;
        groundSpeed = data.groundSpeed;

        windSpeed = data.windSpeed;
        windDirection = data.windDirection;
    }

    inline AttitudeData toAttitudeData() const noexcept
    {
        AttitudeData data;
        toAttitudeData(data);
        return data;
    }

    inline void toAttitudeData(AttitudeData &data) const noexcept
    {
        data.onGround = onGround != 0;

        data.velocityWorldX = velocityWorldX;
        data.velocityWorldY = velocityWorldY;
        data.velocityWorldZ = velocityWorldZ;

        data.accelerationBodyX = accelerationBodyX;
        data.accelerationBodyY = accelerationBodyY;
        data.accelerationBodyZ = accelerationBodyZ;

        data.accelerationWorldX = accelerationWorldX;
        data.accelerationWorldY = accelerationWorldY;
        data.accelerationWorldZ = accelerationWorldZ;

        data.rotationVelocityBodyX = rotationVelocityBodyX;
        data.rotationVelocityBodyY = rotationVelocityBodyY;
        data.rotationVelocityBodyZ = rotationVelocityBodyZ;

        data.gForce = gForce;
        data.incidenceAlpha = incidenceAlpha;

        data.indicatedAirspeed = indicatedAirspeed;
        data.trueAirspeed = trueAirspeed;
        data.groundSpeed = groundSpeed;

        data.windSpeed = windSpeed;
        data.windDirection = windDirection;
    }

    static inline void addToDataDefinition(HANDLE simConnectHandle, ::SIMCONNECT_DATA_DEFINITION_ID dataDefinitionId) noexcept
    {
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::SimOnGround, "Bool", ::SIMCONNECT_DATATYPE_INT32);

        // Velocity World
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::VelocityWorldX, "Feet per Second", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::VelocityWorldY, "Feet per Second", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::VelocityWorldZ, "Feet per Second", ::SIMCONNECT_DATATYPE_FLOAT64);

        // Acceleration Body
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::AccelerationBodyX, "Feet per Second Squared", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::AccelerationBodyY, "Feet per Second Squared", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::AccelerationBodyZ, "Feet per Second Squared", ::SIMCONNECT_DATATYPE_FLOAT64);

        // Acceleration World
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::AccelerationWorldX, "Feet per Second Squared", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::AccelerationWorldY, "Feet per Second Squared", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::AccelerationWorldZ, "Feet per Second Squared", ::SIMCONNECT_DATATYPE_FLOAT64);

        // Rotation Velocity
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::RotationVelocityBodyX, "Feet per Second", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::RotationVelocityBodyY, "Feet per Second", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::RotationVelocityBodyZ, "Feet per Second", ::SIMCONNECT_DATATYPE_FLOAT64);

        // G Force and AoA
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::GForce, "Gforce", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::IncidenceAlpha, "Radians", ::SIMCONNECT_DATATYPE_FLOAT64);

        // Airspeed and Ground speed
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::IndicatedAirspeed, "Knots", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::TrueAirspeed, "Knots", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::GroundSpeed, "Knots", ::SIMCONNECT_DATATYPE_FLOAT64);

        // Wind
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::WindSpeed, "Knots", ::SIMCONNECT_DATATYPE_FLOAT64);
        ::SimConnect_AddToDataDefinition(simConnectHandle, dataDefinitionId, SimVar::WindDirection, "Degrees", ::SIMCONNECT_DATATYPE_FLOAT64);
    }
};
#pragma pack(pop)

#endif // SIMCONNECTATTITUDEINFOH
