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
#ifndef ATTITUDEDATA_H
#define ATTITUDEDATA_H

#include "TimeVariableData.h"
#include "ModelLib.h"

struct MODEL_API AttitudeData final : public TimeVariableData
{
    // Attitude
    double pitch {0.0};
    double bank {0.0};
    double trueHeading {0.0};

    // Velocity
    double velocityBodyX {0.0};
    double velocityBodyY {0.0};
    double velocityBodyZ {0.0};

    double velocityWorldX {0.0};
    double velocityWorldY {0.0};
    double velocityWorldZ {0.0};

    // Acceleration
    double accelerationBodyX {0.0};
    double accelerationBodyY {0.0};
    double accelerationBodyZ {0.0};

    double accelerationWorldX {0.0};
    double accelerationWorldY {0.0};
    double accelerationWorldZ {0.0};

    // G Force
    double gForce {0.0};

    // AoA
    double incidenceAlpha {0.0};

    // Rotation Velocity
    double rotationVelocityBodyX {0.0};
    double rotationVelocityBodyY {0.0};
    double rotationVelocityBodyZ {0.0};

    // Airspeed and Ground Speed
    double indicatedAirspeed {0.0};
    double trueAirspeed {0.0};
    double groundSpeed {0.0};

    // Wind speed and direction
    double windSpeed {0.0};
    double windDirection {0.0};

    bool onGround {false};

    explicit AttitudeData(double pitch = 0.0, double bank = 0.0, double trueHeading = 0.0) noexcept;
};

#endif // ATTITUDEDATA_H
