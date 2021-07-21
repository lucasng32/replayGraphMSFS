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
#ifndef SQLITEAIRCRAFTDAO_H
#define SQLITEAIRCRAFTDAO_H

#include <memory>
#include <vector>

#include <QtGlobal>

#include "../../../../Model/src/Aircraft.h"
#include "../AircraftDaoIntf.h"

class SQLiteAircraftDaoPrivate;

class SQLiteAircraftDao : public AircraftDaoIntf
{
public:
    explicit SQLiteAircraftDao() noexcept;
    virtual ~SQLiteAircraftDao() noexcept;

    virtual bool add(qint64 flightId, int sequenceNumber, Aircraft &aircraft) noexcept override;
    virtual bool getByFlightId(qint64 flightId, std::insert_iterator<std::vector<std::unique_ptr<Aircraft>>> insertIterator) const noexcept override;
    virtual bool adjustAircraftSequenceNumbersByFlightId(qint64 flightId, int sequenceNumber) noexcept override;
    virtual bool deleteAllByFlightId(qint64 flightId) noexcept override;
    virtual bool deleteById(qint64 id) noexcept override;
    virtual bool getAircraftInfosByFlightId(qint64 flightId, std::vector<AircraftInfo> &aircraftInfos) const noexcept override;
    virtual bool updateTimeOffset(qint64 id, qint64 timeOffset) noexcept override;
    virtual bool updateTailNumber(qint64 id, const QString &tailNumber) noexcept override;

private:
    std::unique_ptr<SQLiteAircraftDaoPrivate> d;
};

#endif // SQLITEAIRCRAFTDAO_H
