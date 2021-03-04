#ifndef AIRCRAFTDATA_H
#define AIRCRAFTDATA_H

#include <QtGlobal>

#include "KernelLib.h"

struct KERNEL_API AircraftData
{
    // Position

    // Degrees
    double latitude;
    double longitude;
    double altitude;

    double pitch;
    double bank;
    double heading;

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

    // Flaps & speed brakes
    double leadingEdgeFlapsLeftPercent;
    double leadingEdgeFlapsRightPercent;
    double trailingEdgeFlapsLeftPercent;
    double trailingEdgeFlapsRightPercent;
    double spoilersHandlePosition;
    int flapsHandleIndex;

    // Gears & brakes
    bool gearHandlePosition;
    double waterRudderHandlePosition;
    double brakeLeftPosition;
    double brakeRightPosition;

    // In milliseconds since the start of recording
    qint64 timestamp;

    AircraftData(double latitude = 0.0, double longitude = 0.0, double altitude = 0.0);
    inline bool isNull() const {
        return (latitude == 0.0 && longitude == 0.0 && altitude == 0.0 &&
                pitch == 0.0 && bank == 0.0 && heading == 0.0 && timestamp == 0);
    }

    AircraftData (AircraftData &&) = default;
    AircraftData (const AircraftData &) = default;
    AircraftData &operator= (const AircraftData &) = default;

    static const AircraftData NullAircraftData;
};

#endif // AIRCRAFTDATA_H
