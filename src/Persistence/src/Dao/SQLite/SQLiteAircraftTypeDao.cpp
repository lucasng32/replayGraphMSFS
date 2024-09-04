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
#include <memory>
#include <utility>
#include <vector>
#include <utility>

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#ifdef DEBUG
#include <QDebug>
#endif

#include <Kernel/Enum.h>
#include <Model/AircraftType.h>
#include "SQLiteAircraftTypeDao.h"

namespace
{
    // The initial capacity of the aircraft type vector; happens to be the number of default
    // aircraft types (as per Sky Dolly v0.13), also refer to LogbookMigration.sql
    constexpr int DefaultCapacity = 279;
}

struct SQLiteAircraftTypeDaoPrivate
{
    SQLiteAircraftTypeDaoPrivate(QString connectionName) noexcept
        : connectionName {std::move(connectionName)}
    {}

    QString connectionName;
};

// PUBLIC

SQLiteAircraftTypeDao::SQLiteAircraftTypeDao(QString connectionName) noexcept
    : d {std::make_unique<SQLiteAircraftTypeDaoPrivate>(std::move(connectionName))}
{}

SQLiteAircraftTypeDao::SQLiteAircraftTypeDao(SQLiteAircraftTypeDao &&rhs) noexcept = default;
SQLiteAircraftTypeDao &SQLiteAircraftTypeDao::operator=(SQLiteAircraftTypeDao &&rhs) noexcept = default;
SQLiteAircraftTypeDao::~SQLiteAircraftTypeDao() = default;

bool SQLiteAircraftTypeDao::upsert(const AircraftType &aircraftType) const noexcept
{
    const auto db {QSqlDatabase::database(d->connectionName)};
    QSqlQuery query {db};
    query.prepare(
        "insert into aircraft_type (type, "
        "category, "
        "wing_span, "
        "engine_type, "
        "nof_engines, "
        "design_cruise_altitude, "
        "design_climb_speed, "
        "design_cruise_speed, "
        "flaps_full_stall_speed, "
        "flaps_up_stall_speed, "
        "design_takeoff_speed, "
        "stall_alpha, "
        "static_pitch, "
        "typical_descent_rate, "
        "zero_lift_alpha"
        ") "
        "values(:type, "
        ":category, "
        ":wing_span, "
        ":engine_type, "
        ":nof_engines, "
        ":design_cruise_altitude, "
        ":design_climb_speed, "
        ":design_cruise_speed, "
        ":flaps_full_stall_speed, "
        ":flaps_up_stall_speed, "
        ":design_takeoff_speed, "
        ":stall_alpha, "
        ":static_pitch, "
        ":typical_descent_rate, "
        ":zero_lift_alpha"
        ") "
        "on conflict(type) "
        "do update "
        "set category = excluded.category, "
        "    wing_span = excluded.wing_span, "
        "    engine_type = excluded.engine_type, "
        "    nof_engines = excluded.nof_engines, "
        "    design_cruise_altitude = excluded.design_cruise_altitude, "
        "    design_climb_speed = excluded.design_climb_speed, "
        "    design_cruise_speed = excluded.design_cruise_speed, "
        "    flaps_full_stall_speed = excluded.flaps_full_stall_speed, "
        "    flaps_up_stall_speed = excluded.flaps_up_stall_speed, "
        "    design_takeoff_speed = excluded.design_takeoff_speed, "
        "    stall_alpha = excluded.stall_alpha, "
        "    static_pitch = excluded.static_pitch, "
        "    typical_descent_rate = excluded.typical_descent_rate, "
        "    zero_lift_alpha = excluded.zero_lift_alpha; "
    );

    query.bindValue(":type", aircraftType.type);
    query.bindValue(":category", aircraftType.category);
    query.bindValue(":wing_span", aircraftType.wingSpan);
    query.bindValue(":engine_type", Enum::underly(aircraftType.engineType));
    query.bindValue(":nof_engines", aircraftType.numberOfEngines);
    query.bindValue(":design_cruise_altitude", aircraftType.designCruiseAltitude);
    query.bindValue(":design_climb_speed", aircraftType.designClimbSpeed);
    query.bindValue(":design_cruise_speed", aircraftType.designCruiseSpeed);
    query.bindValue(":flaps_full_stall_speed", aircraftType.flapsFullStallSpeed);
    query.bindValue(":flaps_up_stall_speed", aircraftType.flapsUpStallSpeed);
    query.bindValue(":design_takeoff_speed", aircraftType.designTakeoffSpeed);
    query.bindValue(":stall_alpha", aircraftType.stallAlpha);
    query.bindValue(":static_pitch", aircraftType.staticPitch);
    query.bindValue(":zero_lift_alpha", aircraftType.zeroLiftAlpha);

    const bool ok = query.exec();
#ifdef DEBUG
    if (!ok) {
        qDebug() << "SQLiteAircraftTypeDao::upsert: SQL error" << query.lastError().text() << "- error code:" << query.lastError().nativeErrorCode();
    }
#endif
    return ok;
}

AircraftType SQLiteAircraftTypeDao::getByType(const QString &type, bool *ok) const noexcept
{
    AircraftType aircraftType;
    const auto db {QSqlDatabase::database(d->connectionName)};
    QSqlQuery query {db};
    query.setForwardOnly(true);
    query.prepare(
        "select * "
         "from   aircraft_type at "
         "where  at.type = :type;"
    );

    query.bindValue(":type", type);
    aircraftType.type = type;
    bool success = query.exec();
    if (success) {
        const QSqlRecord record = query.record();
        success = query.next();
        if (success) {
            const auto categoryIdx = record.indexOf("category");
            const auto wingSpanIdx = record.indexOf("wing_span");
            const auto engineTypeIdx = record.indexOf("engine_type");
            const auto nofEnginesIdx = record.indexOf("nof_engines");

            const auto designCruiseAltitudeIdx = record.indexOf("design_cruise_altitude");
            const auto designClimbSpeedIdx = record.indexOf("design_climb_speed");
            const auto designCruiseSpeedIdx = record.indexOf("design_cruise_speed");
            const auto flapsFullStallSpeedIdx = record.indexOf("flaps_full_stall_speed");
            const auto flapsUpStallSpeedIdx = record.indexOf("flaps_up_stall_speed");
            const auto designTakeoffSpeedIdx = record.indexOf("design_takeoff_speed");
            const auto stallAlphaIdx = record.indexOf("stall_alpha");
            const auto staticPitchIdx = record.indexOf("static_pitch");
            const auto typicalDescentRateIdx = record.indexOf("typical_descent_rate");
            const auto zeroLiftAlphaIdx = record.indexOf("zero_lift_alpha");

            aircraftType.category = query.value(categoryIdx).toString();
            aircraftType.wingSpan = query.value(wingSpanIdx).toInt();
            const auto enumValue = query.value(engineTypeIdx).toInt();
            aircraftType.engineType = Enum::contains<SimType::EngineType>(enumValue) ? static_cast<SimType::EngineType>(enumValue) : SimType::EngineType::First;
            aircraftType.numberOfEngines = query.value(nofEnginesIdx).toInt();

            aircraftType.designCruiseAltitude = query.value(designCruiseAltitudeIdx).toDouble();
            aircraftType.designClimbSpeed = query.value(designClimbSpeedIdx).toDouble();
            aircraftType.designCruiseSpeed = query.value(designCruiseSpeedIdx).toDouble();
            aircraftType.flapsFullStallSpeed = query.value(flapsFullStallSpeedIdx).toDouble();
            aircraftType.flapsUpStallSpeed = query.value(flapsUpStallSpeedIdx).toDouble();
            aircraftType.designTakeoffSpeed = query.value(designTakeoffSpeedIdx).toDouble();
            aircraftType.stallAlpha = query.value(stallAlphaIdx).toDouble();
            aircraftType.staticPitch = query.value(staticPitchIdx).toDouble();
            aircraftType.typicalDescentRate = query.value(typicalDescentRateIdx).toDouble();
            aircraftType.zeroLiftAlpha = query.value(zeroLiftAlphaIdx).toDouble();

        }
    }
#ifdef DEBUG
    else {
        qDebug() << "SQLiteAircraftTypeDao::getByType: SQL error" << query.lastError().text() << "- error code:" << query.lastError().nativeErrorCode();
    }
#endif
    if (ok != nullptr) {
        *ok = success;
    }
    return aircraftType;
}

std::vector<AircraftType> SQLiteAircraftTypeDao::getAll(bool *ok) const noexcept
{
    std::vector<AircraftType> aircraftTypes;
    const auto db {QSqlDatabase::database(d->connectionName)};
    QSqlQuery query {db};
    query.setForwardOnly(true);
    query.prepare(
        "select * "
        "from   aircraft_type at "
        "order by at.type asc;"
    );
    const bool success = query.exec();
    if (success) {
        const bool querySizeFeature = db.driver()->hasFeature(QSqlDriver::QuerySize);
        if (querySizeFeature) {
            aircraftTypes.reserve(query.size());
        } else {
            aircraftTypes.reserve(::DefaultCapacity);
        }

        QSqlRecord record = query.record();
        const auto typeIdx = record.indexOf("type");
        const auto categoryIdx = record.indexOf("category");
        const auto wingSpanIdx = record.indexOf("wing_span");
        const auto engineTypeIdx = record.indexOf("engine_type");
        const auto nofEnginesIdx = record.indexOf("nof_engines");

        const auto designCruiseAltitudeIdx = record.indexOf("design_cruise_altitude");
        const auto designClimbSpeedIdx = record.indexOf("design_climb_speed");
        const auto designCruiseSpeedIdx = record.indexOf("design_cruise_speed");
        const auto flapsFullStallSpeedIdx = record.indexOf("flaps_full_stall_speed");
        const auto flapsUpStallSpeedIdx = record.indexOf("flaps_up_stall_speed");
        const auto designTakeoffSpeedIdx = record.indexOf("design_takeoff_speed");
        const auto stallAlphaIdx = record.indexOf("stall_alpha");
        const auto staticPitchIdx = record.indexOf("static_pitch");
        const auto typicalDescentRateIdx = record.indexOf("typical_descent_rate");
        const auto zeroLiftAlphaIdx = record.indexOf("zero_lift_alpha");

        while (query.next()) {
            const QString type = query.value(typeIdx).toString();
            const QString category = query.value(categoryIdx).toString();
            const auto wingSpan = query.value(wingSpanIdx).toInt();
            const auto enumValue = query.value(engineTypeIdx).toInt();
            const SimType::EngineType engineType = Enum::contains<SimType::EngineType>(enumValue) ? static_cast<SimType::EngineType>(enumValue) : SimType::EngineType::First;
            const auto numberOfEngines = query.value(nofEnginesIdx).toInt();

            const auto designCruiseAltitude = query.value(designCruiseAltitudeIdx).toDouble();
            const auto designClimbSpeed = query.value(designClimbSpeedIdx).toDouble();
            const auto designCruiseSpeed = query.value(designCruiseSpeedIdx).toDouble();
            const auto flapsFullStallSpeed = query.value(flapsFullStallSpeedIdx).toDouble();
            const auto flapsUpStallSpeed = query.value(flapsUpStallSpeedIdx).toDouble();
            const auto designTakeoffSpeed = query.value(designTakeoffSpeedIdx).toDouble();
            const auto stallAlpha = query.value(stallAlphaIdx).toDouble();
            const auto staticPitch = query.value(staticPitchIdx).toDouble();
            const auto typicalDescentRate = query.value(typicalDescentRateIdx).toDouble();
            const auto zeroLiftAlpha = query.value(zeroLiftAlphaIdx).toDouble();

            aircraftTypes.emplace_back(type, category, wingSpan, engineType, numberOfEngines,
                designCruiseAltitude, designClimbSpeed, designCruiseSpeed, flapsFullStallSpeed, flapsUpStallSpeed,
                                       designTakeoffSpeed, stallAlpha, staticPitch, typicalDescentRate, zeroLiftAlpha);
        }
#ifdef DEBUG
    } else {
            qDebug() << "SQLiteAircraftTypeDao::getAll: SQL error" << query.lastError().text() << "- error code:" << query.lastError().nativeErrorCode();
#endif
    }

    if (ok != nullptr) {
        *ok = success;
    }
    return aircraftTypes;
}

bool SQLiteAircraftTypeDao::exists(const QString &type) const noexcept
{
    bool exists {false};
    const auto db {QSqlDatabase::database(d->connectionName)};
    QSqlQuery query {db};
    query.setForwardOnly(true);
    query.prepare(
        "select count(*) "
        "from   aircraft_type at "
        "where  at.type = :type "
        "limit 1;"
    );

    query.bindValue(":type", type);
    const bool ok = query.exec();
    if (ok && query.next()) {
        const auto count = query.value(0).toInt();
        exists = count > 0;
    }
#ifdef DEBUG
    else {
        qDebug() << "SQLiteAircraftTypeDao::exists: SQL error" << query.lastError().text() << "- error code:" << query.lastError().nativeErrorCode();
    }
#endif

    return ok && exists;
}
