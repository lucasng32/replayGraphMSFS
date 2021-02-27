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

    // General engine (percent)
    double throttleLeverPosition1;
    double throttleLeverPosition2;
    double throttleLeverPosition3;
    double throttleLeverPosition4;

    double spoilersHandlePosition;
    int flapsHandleIndex;

    // In milliseconds since the start of recording
    qint64 timestamp;

    AircraftData(double latitude = 0.0, double longitude = 0.0, double altitude = 0.0);
    bool isValid() const;

    AircraftData (AircraftData &&) = default;
    AircraftData (const AircraftData &) = default;
    AircraftData &operator= (const AircraftData &) = default;

    static const AircraftData NullAircraftData;
};

#endif // AIRCRAFTDATA_H
