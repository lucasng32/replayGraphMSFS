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

#include <QObject>
#include <QString>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QSqlRecord>

#include "../../../../Kernel/src/Enum.h"
#include "../../../../Model/src/PrimaryFlightControlData.h"
#include "../../ConnectionManager.h"
#include "SQLitePrimaryFlightControlDao.h"

class SQLitePrimaryFlightControlDaoPrivate
{
public:
    SQLitePrimaryFlightControlDaoPrivate() noexcept
    {}

    std::unique_ptr<QSqlQuery> insertQuery;
    std::unique_ptr<QSqlQuery> selectByAircraftIdQuery;
    std::unique_ptr<QSqlQuery> removeByFlightIdQuery;
    std::unique_ptr<QSqlQuery> removeByIdQuery;

    void initQueries()
    {
        if (insertQuery == nullptr) {
            insertQuery = std::make_unique<QSqlQuery>();
            insertQuery->prepare(
"insert into primary_flight_control ("
"  aircraft_id,"
"  timestamp,"
"  rudder_position,"
"  elevator_position,"
"  aileron_position"
") values ("
" :aircraft_id,"
" :timestamp,"
" :rudder_position,"
" :elevator_position,"
" :aileron_position"
");");
        }
        if (selectByAircraftIdQuery == nullptr) {
            selectByAircraftIdQuery = std::make_unique<QSqlQuery>();
            selectByAircraftIdQuery->prepare(
"select * "
"from   primary_flight_control pfc "
"where  pfc.aircraft_id = :aircraft_id "
"order by pfc.timestamp asc;");
        }
        if (removeByFlightIdQuery == nullptr) {
            removeByFlightIdQuery = std::make_unique<QSqlQuery>();
            removeByFlightIdQuery->prepare(
"delete "
"from   primary_flight_control "
"where  aircraft_id in (select a.id "
"                       from aircraft a"
"                       where a.flight_id = :flight_id"
"                      );");
        }
        if (removeByIdQuery == nullptr) {
            removeByIdQuery = std::make_unique<QSqlQuery>();
            removeByIdQuery->prepare(
"delete "
"from   primary_flight_control "
"where  aircraft_id = :aircraft_id;");
        }
    }

    void resetQueries() noexcept
    {
        insertQuery = nullptr;
        selectByAircraftIdQuery = nullptr;
        removeByFlightIdQuery = nullptr;
        removeByIdQuery = nullptr;
    }
};

// PUBLIC

SQLitePrimaryFlightControlDao::SQLitePrimaryFlightControlDao(QObject *parent) noexcept
    : QObject(parent),
      d(std::make_unique<SQLitePrimaryFlightControlDaoPrivate>())
{
    frenchConnection();
}

SQLitePrimaryFlightControlDao::~SQLitePrimaryFlightControlDao() noexcept
{}

bool SQLitePrimaryFlightControlDao::add(qint64 aircraftId, const PrimaryFlightControlData &primaryFlightControlData)  noexcept
{
    d->initQueries();
    d->insertQuery->bindValue(":aircraft_id", aircraftId);
    d->insertQuery->bindValue(":timestamp", primaryFlightControlData.timestamp);
    d->insertQuery->bindValue(":rudder_position", primaryFlightControlData.rudderPosition);
    d->insertQuery->bindValue(":elevator_position", primaryFlightControlData.elevatorPosition);
    d->insertQuery->bindValue(":aileron_position", primaryFlightControlData.aileronPosition);

    bool ok = d->insertQuery->exec();
#ifdef DEBUG
    if (!ok) {
        qDebug("SQLitePrimaryFlightControlDao::add: SQL error: %s", qPrintable(d->insertQuery->lastError().databaseText() + " - error code: " + d->insertQuery->lastError().nativeErrorCode()));
    }
#endif
    return ok;
}

bool SQLitePrimaryFlightControlDao::getByAircraftId(qint64 aircraftId, QVector<PrimaryFlightControlData> &primaryFlightControlData) const noexcept
{
    d->initQueries();
    d->selectByAircraftIdQuery->bindValue(":aircraft_id", aircraftId);
    bool ok = d->selectByAircraftIdQuery->exec();
    if (ok) {
        primaryFlightControlData.clear();
        QSqlRecord record = d->selectByAircraftIdQuery->record();
        const int timestampIdx = record.indexOf("timestamp");
        const int rudderPositionIdx = record.indexOf("rudder_position");
        const int elevatorPositionIdx = record.indexOf("elevator_position");
        const int aileronPositionIdx = record.indexOf("aileron_position");
        while (d->selectByAircraftIdQuery->next()) {

            PrimaryFlightControlData data;

            data.timestamp = d->selectByAircraftIdQuery->value(timestampIdx).toLongLong();
            data.rudderPosition = d->selectByAircraftIdQuery->value(rudderPositionIdx).toDouble();
            data.elevatorPosition = d->selectByAircraftIdQuery->value(elevatorPositionIdx).toDouble();
            data.aileronPosition = d->selectByAircraftIdQuery->value(aileronPositionIdx).toDouble();

            primaryFlightControlData.append(data);
        }
#ifdef DEBUG
    } else {
        qDebug("SQLitePrimaryFlightControlDao::getByAircraftId: SQL error: %s", qPrintable(d->selectByAircraftIdQuery->lastError().databaseText() + " - error code: " + d->selectByAircraftIdQuery->lastError().nativeErrorCode()));
#endif
    }

    return ok;
}

bool SQLitePrimaryFlightControlDao::removeByFlightId(qint64 flightId) noexcept
{
    d->initQueries();
    d->removeByFlightIdQuery->bindValue(":flight_id", flightId);
    bool ok = d->removeByFlightIdQuery->exec();
#ifdef DEBUG
    if (!ok) {
        qDebug("SQLitePrimaryFlightControlDao::removeByFlightId: SQL error: %s", qPrintable(d->removeByFlightIdQuery->lastError().databaseText() + " - error code: " + d->removeByFlightIdQuery->lastError().nativeErrorCode()));
    }
#endif
    return ok;
}

bool SQLitePrimaryFlightControlDao::removeByAircraftId(qint64 aircraftId) noexcept
{
    d->initQueries();
    d->removeByIdQuery->bindValue(":aircraft_id", aircraftId);
    bool ok = d->removeByIdQuery->exec();
#ifdef DEBUG
    if (!ok) {
        qDebug("SQLitePrimaryFlightControlDao::removeByAircraftId: SQL error: %s", qPrintable(d->removeByIdQuery->lastError().databaseText() + " - error code: " + d->removeByIdQuery->lastError().nativeErrorCode()));
    }
#endif
    return true;
}

// PRIVATE

void SQLitePrimaryFlightControlDao::frenchConnection() noexcept
{
    connect(&ConnectionManager::getInstance(), &ConnectionManager::connectionChanged,
            this, &SQLitePrimaryFlightControlDao::handleConnectionChanged);
}

// PRIVATE SLOTS

void SQLitePrimaryFlightControlDao::handleConnectionChanged() noexcept
{
    d->resetQueries();
}
