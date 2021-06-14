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
#include <QTimeZone>

#include "../../../../Kernel/src/Enum.h"
#include "../../../../Model/src/Aircraft.h"
#include "../../../../Model/src/AircraftInfo.h"
#include "../../../../Model/src/Position.h"
#include "../../../../Model/src/PositionData.h"
#include "../../../../Model/src/Engine.h"
#include "../../../../Model/src/EngineData.h"
#include "../../../../Model/src/PrimaryFlightControl.h"
#include "../../../../Model/src/PrimaryFlightControlData.h"
#include "../../../../Model/src/SecondaryFlightControl.h"
#include "../../../../Model/src/SecondaryFlightControlData.h"
#include "../../../../Model/src/AircraftHandle.h"
#include "../../../../Model/src/AircraftHandleData.h"
#include "../../../../Model/src/Light.h"
#include "../../../../Model/src/LightData.h"
#include "../../../../Model/src/FlightPlan.h"
#include "../../../../Model/src/Waypoint.h"
#include "../../Dao/FlightDaoIntf.h"
#include "../../Dao/AircraftDaoIntf.h"
#include "../../Dao/PositionDaoIntf.h"
#include "../../Dao/EngineDaoIntf.h"
#include "../../Dao/PrimaryFlightControlDaoIntf.h"
#include "../../Dao/SecondaryFlightControlDaoIntf.h"
#include "../../Dao/HandleDaoIntf.h"
#include "../../Dao/LightDaoIntf.h"
#include "../../Dao/WaypointDaoIntf.h"
#include "../../Dao/DaoFactory.h"
#include "../../ConnectionManager.h"
#include "SQLiteAircraftDao.h"

class SQLiteAircraftDaoPrivate
{
public:
    SQLiteAircraftDaoPrivate() noexcept
        : daoFactory(std::make_unique<DaoFactory>(DaoFactory::DbType::SQLite)),
          positionDao(daoFactory->createPositionDao()),
          engineDao(daoFactory->createEngineDao()),
          primaryFlightControlDao(daoFactory->createPrimaryFlightControlDao()),
          secondaryFlightControlDao(daoFactory->createSecondaryFlightControlDao()),
          handleDao(daoFactory->createHandleDao()),
          lightDao(daoFactory->createLightDao()),
          waypointDao(daoFactory->createFlightPlanDao())
    {}

    std::unique_ptr<QSqlQuery> insertQuery;
    std::unique_ptr<QSqlQuery> selectByFlightIdQuery;
    std::unique_ptr<QSqlQuery> adjustAircraftSequenceNumbersQuery;
    std::unique_ptr<QSqlQuery> removeByFlightIdQuery;
    std::unique_ptr<QSqlQuery> removeById;

    std::unique_ptr<DaoFactory> daoFactory;
    std::unique_ptr<PositionDaoIntf> positionDao;
    std::unique_ptr<EngineDaoIntf> engineDao;
    std::unique_ptr<PrimaryFlightControlDaoIntf> primaryFlightControlDao;
    std::unique_ptr<SecondaryFlightControlDaoIntf> secondaryFlightControlDao;
    std::unique_ptr<HandleDaoIntf> handleDao;
    std::unique_ptr<LightDaoIntf> lightDao;
    std::unique_ptr<WaypointDaoIntf> waypointDao;

    void initQueries()
    {
        if (insertQuery == nullptr) {
            insertQuery = std::make_unique<QSqlQuery>();
            insertQuery->prepare(
"insert into aircraft ("
"  id,"
"  flight_id,"
"  seq_nr,"
"  start_date,"
"  end_date,"
"  type,"
"  tail_number,"
"  airline,"
"  flight_number,"
"  category,"
"  initial_airspeed,"
"  wing_span,"
"  engine_type,"
"  nof_engines,"
"  altitude_above_ground,"
"  start_on_ground"
") values ("
"  null,"
" :flight_id,"
" :seq_nr,"
" :start_date,"
" :end_date,"
" :type,"
" :tail_number,"
" :airline,"
" :flight_number,"
" :category,"
" :initial_airspeed,"
" :wing_span,"
" :engine_type,"
" :nof_engines,"
" :altitude_above_ground,"
" :start_on_ground"
");");
        }
        if (selectByFlightIdQuery == nullptr) {
            selectByFlightIdQuery = std::make_unique<QSqlQuery>();
            selectByFlightIdQuery->setForwardOnly(true);
            selectByFlightIdQuery->prepare(
"select * "
"from   aircraft a "
"where  a.flight_id = :flight_id "
"order by a.seq_nr;");
        }
        if (adjustAircraftSequenceNumbersQuery == nullptr) {
            adjustAircraftSequenceNumbersQuery = std::make_unique<QSqlQuery>();
            adjustAircraftSequenceNumbersQuery->prepare(
"update aircraft "
"set    seq_nr = seq_nr - 1 "
"where flight_id = :flight_id "
"and   seq_nr    > :seq_nr;");
        }
        if (removeByFlightIdQuery == nullptr) {
            removeByFlightIdQuery = std::make_unique<QSqlQuery>();
            removeByFlightIdQuery->prepare(
"delete "
"from   aircraft "
"where  flight_id = :flight_id;");
        }
        if (removeById == nullptr) {
            removeById = std::make_unique<QSqlQuery>();
            removeById->prepare(
"delete "
"from   aircraft "
"where  id = :id;");
        }
    }

    void resetQueries() noexcept
    {
        insertQuery = nullptr;
        selectByFlightIdQuery = nullptr;
        adjustAircraftSequenceNumbersQuery = nullptr;
        removeByFlightIdQuery = nullptr;
        removeById = nullptr;
    }
};

// PUBLIC

SQLiteAircraftDao::SQLiteAircraftDao(QObject *parent) noexcept
    : QObject(parent),
      d(std::make_unique<SQLiteAircraftDaoPrivate>())
{
    frenchConnection();
}

SQLiteAircraftDao::~SQLiteAircraftDao() noexcept
{}

bool SQLiteAircraftDao::add(qint64 flightId, int sequenceNumber, Aircraft &aircraft)  noexcept
{
    d->initQueries();
    const AircraftInfo &info = aircraft.getAircraftInfoConst();
    d->insertQuery->bindValue(":flight_id", flightId);
    d->insertQuery->bindValue(":seq_nr", sequenceNumber);
    d->insertQuery->bindValue(":start_date", info.startDate.toUTC());
    d->insertQuery->bindValue(":end_date", info.endDate.toUTC());
    d->insertQuery->bindValue(":type", info.type);
    d->insertQuery->bindValue(":tail_number", info.tailNumber);
    d->insertQuery->bindValue(":airline", info.airline);
    d->insertQuery->bindValue(":flight_number", info.flightNumber);
    d->insertQuery->bindValue(":category", info.category);
    d->insertQuery->bindValue(":initial_airspeed", info.initialAirspeed);
    d->insertQuery->bindValue(":wing_span", info.wingSpan);
    d->insertQuery->bindValue(":engine_type", Enum::toUnderlyingType(info.engineType));
    d->insertQuery->bindValue(":nof_engines", info.numberOfEngines);
    d->insertQuery->bindValue(":altitude_above_ground", info.altitudeAboveGround);
    d->insertQuery->bindValue(":start_on_ground", info.startOnGround);

    bool ok = d->insertQuery->exec();
    if (ok) {
        qint64 id = d->insertQuery->lastInsertId().toLongLong(&ok);
        aircraft.setId(id);
#ifdef DEBUG
    } else {
        qDebug("SQLiteAircraftDao::add: SQL error: %s", qPrintable(d->insertQuery->lastError().databaseText() + " - error code: " + d->insertQuery->lastError().nativeErrorCode()));
#endif
    }
    if (ok) {
        for (const PositionData &data : aircraft.getPositionConst().getAllConst()) {
            ok = d->positionDao->add(aircraft.getId(), data);
            if (!ok) {
                break;
            }
        }
    }
    if (ok) {
        for (const EngineData &data : aircraft.getEngineConst().getAllConst()) {
            ok = d->engineDao->add(aircraft.getId(), data);
            if (!ok) {
                break;
            }
        }
    }
    if (ok) {
        for (const PrimaryFlightControlData &data : aircraft.getPrimaryFlightControlConst().getAllConst()) {
            ok = d->primaryFlightControlDao->add(aircraft.getId(), data);
            if (!ok) {
                break;
            }
        }
    }
    if (ok) {
        for (const SecondaryFlightControlData &data : aircraft.getSecondaryFlightControlConst().getAllConst()) {
            ok = d->secondaryFlightControlDao->add(aircraft.getId(), data);
            if (!ok) {
                break;
            }
        }
    }
    if (ok) {
        for (const AircraftHandleData &data : aircraft.getAircraftHandleConst().getAllConst()) {
            ok = d->handleDao->add(aircraft.getId(), data);
            if (!ok) {
                break;
            }
        }
    }
    if (ok) {
        for (const LightData &data : aircraft.getLightConst().getAllConst()) {
            ok = d->lightDao->add(aircraft.getId(), data);
            if (!ok) {
                break;
            }
        }
    }
    if (ok) {
        ok = d->waypointDao->add(aircraft.getId(), aircraft.getFlightPlanConst().getAllConst());
    }
    return ok;
}

bool SQLiteAircraftDao::getByFlightId(qint64 flightId, std::vector<std::unique_ptr<Aircraft>> &aircrafts) const noexcept
{
    std::vector<AircraftInfo> aircraftInfos;
    bool ok = getAircraftInfosByFlightId(flightId, aircraftInfos);
    if (ok) {
        aircrafts.clear();
        for (const AircraftInfo &info: aircraftInfos) {
            std::unique_ptr<Aircraft> aircraft = std::make_unique<Aircraft>();
            aircraft->setId(info.aircraftId);
            aircraft->setAircraftInfo(info);

            ok = d->positionDao->getByAircraftId(aircraft->getId(), aircraft->getPosition().getAll());
            if (ok) {
                ok = d->engineDao->getByAircraftId(aircraft->getId(), aircraft->getEngine().getAll());
            }
            if (ok) {
                ok = d->primaryFlightControlDao->getByAircraftId(aircraft->getId(), aircraft->getPrimaryFlightControl().getAll());
            }
            if (ok) {
                ok = d->secondaryFlightControlDao->getByAircraftId(aircraft->getId(), aircraft->getSecondaryFlightControl().getAll());
            }
            if (ok) {
                ok = d->handleDao->getByAircraftId(aircraft->getId(), aircraft->getAircraftHandle().getAll());
            }
            if (ok) {
                ok = d->lightDao->getByAircraftId(aircraft->getId(), aircraft->getLight().getAll());
            }
            if (ok) {
                ok = d->waypointDao->getByAircraftId(aircraft->getId(), aircraft->getFlightPlan());
            }
            if (ok) {
                emit aircraft->dataChanged();
                aircrafts.push_back(std::move(aircraft));
            }
        }
#ifdef DEBUG
    } else {
        qDebug("SQLiteAircraftDao::getByFlightId: SQL error: %s", qPrintable(d->selectByFlightIdQuery->lastError().databaseText() + " - error code: " + d->selectByFlightIdQuery->lastError().nativeErrorCode()));
#endif
    }

    return ok;
}

bool SQLiteAircraftDao::adjustAircraftSequenceNumbersByFlightId(qint64 flightId, int sequenceNumber) noexcept
{
    d->initQueries();
    d->adjustAircraftSequenceNumbersQuery->bindValue(":flight_id", flightId);
    d->adjustAircraftSequenceNumbersQuery->bindValue(":seq_nr", sequenceNumber);
    bool ok = d->adjustAircraftSequenceNumbersQuery->exec();
#ifdef DEBUG
    if (!ok) {
        qDebug("SQLiteAircraftDao::adjustAircraftSequenceNumbersByIndex: SQL error: %s", qPrintable(d->adjustAircraftSequenceNumbersQuery->lastError().databaseText() + " - error code: " + d->adjustAircraftSequenceNumbersQuery->lastError().nativeErrorCode()));
    }
#endif
    return ok;
}

bool SQLiteAircraftDao::removeAllByFlightId(qint64 flightId) noexcept
{
    d->initQueries();
    // Delete "bottom-up" in order not to violate foreign key constraints
    bool ok = d->positionDao->removeByFlightId(flightId);
    if (ok) {
        ok = d->engineDao->removeByFlightId(flightId);
    }
    if (ok) {
        ok = d->primaryFlightControlDao->removeByFlightId(flightId);
    }
    if (ok) {
        ok = d->secondaryFlightControlDao->removeByFlightId(flightId);
    }
    if (ok) {
        ok = d->handleDao->removeByFlightId(flightId);
    }
    if (ok) {
        ok = d->lightDao->removeByFlightId(flightId);
    }
    if (ok) {
        ok = d->waypointDao->removeByFlightId(flightId);
    }
    if (ok) {
        d->removeByFlightIdQuery->bindValue(":flight_id", flightId);
        ok = d->removeByFlightIdQuery->exec();
#ifdef DEBUG
        if (!ok) {
            qDebug("SQLiteAircraftDao::removeByFlightId: SQL error: %s", qPrintable(d->removeByFlightIdQuery->lastError().databaseText() + " - error code: " + d->removeByFlightIdQuery->lastError().nativeErrorCode()));
        }
#endif
    }

    return ok;
}

bool SQLiteAircraftDao::removeById(qint64 id) noexcept
{
    d->initQueries();
    // Delete "bottom-up" in order not to violate foreign key constraints
    bool ok = d->positionDao->removeByAircraftId(id);
    if (ok) {
        ok = d->engineDao->removeByAircraftId(id);
    }
    if (ok) {
        ok = d->primaryFlightControlDao->removeByAircraftId(id);
    }
    if (ok) {
        ok = d->secondaryFlightControlDao->removeByAircraftId(id);
    }
    if (ok) {
        ok = d->handleDao->removeByAircraftId(id);
    }
    if (ok) {
        ok = d->lightDao->removeByAircraftId(id);
    }
    if (ok) {
        ok = d->waypointDao->removeByAircraftId(id);
    }
    if (ok) {
        d->removeById->bindValue(":id", id);
        ok = d->removeById->exec();
#ifdef DEBUG
        if (!ok) {
            qDebug("SQLiteAircraftDao::removeById: SQL error: %s", qPrintable(d->removeById->lastError().databaseText() + " - error code: " + d->removeById->lastError().nativeErrorCode()));
        }
#endif
    }

    return ok;
}

bool SQLiteAircraftDao::getAircraftInfosByFlightId(qint64 flightId, std::vector<AircraftInfo> &aircraftInfos) const noexcept
{
    d->initQueries();
    d->selectByFlightIdQuery->bindValue(":flight_id", flightId);
    bool ok = d->selectByFlightIdQuery->exec();
    if (ok) {
        aircraftInfos.clear();
        QSqlRecord record = d->selectByFlightIdQuery->record();
        const int idIdx = record.indexOf("id");
        const int startDateIdx = record.indexOf("start_date");
        const int endDateIdx = record.indexOf("end_date");
        const int typeIdx = record.indexOf("type");
        const int tailNumberIdx = record.indexOf("tail_number");
        const int airlineIdx = record.indexOf("airline");
        const int flightNumberIdx = record.indexOf("flight_number");
        const int categoryIdx = record.indexOf("category");
        const int initialAirspeedIdx = record.indexOf("initial_airspeed");
        const int wingSpanIdx = record.indexOf("wing_span");
        const int engineTypeIdx = record.indexOf("engine_type");
        const int nofEnginesIdx = record.indexOf("nof_engines");
        const int airCraftAltitudeAboveGroundIdx = record.indexOf("altitude_above_ground");
        const int startOnGroundIdx = record.indexOf("start_on_ground");
        while (d->selectByFlightIdQuery->next()) {
            AircraftInfo info(d->selectByFlightIdQuery->value(idIdx).toLongLong());
            QDateTime dateTime = d->selectByFlightIdQuery->value(startDateIdx).toDateTime();
            dateTime.setTimeZone(QTimeZone::utc());
            info.startDate = dateTime.toLocalTime();
            dateTime = d->selectByFlightIdQuery->value(endDateIdx).toDateTime();
            dateTime.setTimeZone(QTimeZone::utc());
            info.endDate = dateTime.toLocalTime();
            info.type = d->selectByFlightIdQuery->value(typeIdx).toString();
            info.tailNumber = d->selectByFlightIdQuery->value(tailNumberIdx).toString();
            info.airline = d->selectByFlightIdQuery->value(airlineIdx).toString();
            info.flightNumber = d->selectByFlightIdQuery->value(flightNumberIdx).toString();
            info.category = d->selectByFlightIdQuery->value(categoryIdx).toString();
            info.initialAirspeed = d->selectByFlightIdQuery->value(initialAirspeedIdx).toInt();
            info.wingSpan = d->selectByFlightIdQuery->value(wingSpanIdx).toInt();
            info.engineType = static_cast<SimType::EngineType>(d->selectByFlightIdQuery->value(engineTypeIdx).toInt());
            info.numberOfEngines = d->selectByFlightIdQuery->value(nofEnginesIdx).toInt();
            info.altitudeAboveGround = d->selectByFlightIdQuery->value(airCraftAltitudeAboveGroundIdx).toInt();
            info.startOnGround = d->selectByFlightIdQuery->value(startOnGroundIdx).toBool();

            aircraftInfos.push_back(info);
        }
    }
    return ok;
}

// PRIVATE

void SQLiteAircraftDao::frenchConnection() noexcept
{
    connect(&ConnectionManager::getInstance(), &ConnectionManager::connectionChanged,
            this, &SQLiteAircraftDao::handleConnectionChanged);
}

// PRIVATE SLOTS

void SQLiteAircraftDao::handleConnectionChanged() noexcept
{
    d->resetQueries();
}
