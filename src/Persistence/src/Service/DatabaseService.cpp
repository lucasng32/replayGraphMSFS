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
#include <QFileInfo>
#include <QDir>

#include "../../../Kernel/src/Settings.h"
#include "../ConnectionManager.h"
#include "DatabaseService.h"

namespace
{
    constexpr int MaxBackupIndex = 1024;
}

// PUBLIC

DatabaseService::DatabaseService() noexcept
{}

DatabaseService::~DatabaseService() noexcept
{}

bool DatabaseService::connectDb() noexcept
{
    ConnectionManager &connectionManager = ConnectionManager::getInstance();
    const QString &logbookDirectoryPath = Settings::getInstance().getLogbookDirectoryPath();
    QFileInfo info(logbookDirectoryPath);
    QDir dir(logbookDirectoryPath);
    bool ok = info.exists();
    if (!ok) {
        ok = dir.mkpath(logbookDirectoryPath);
    }
    if (ok) {
        QString logbookPath = logbookDirectoryPath + "/" + dir.dirName() + LogbookExtension;
        ok = connectionManager.connectDb(logbookPath);
        if (ok) {
            ok = connectionManager.migrate();
        }
    }
    return ok;
}

void DatabaseService::disconnectDb() noexcept
{
    return ConnectionManager::getInstance().disconnectDb();
}

bool DatabaseService::isConnected() const noexcept
{
    return ConnectionManager::getInstance().isConnected();
}

const QString &DatabaseService::getLogbookPath() const noexcept
{
    return ConnectionManager::getInstance().getLogbookPath();
}

bool DatabaseService::optimise() noexcept
{
    return ConnectionManager::getInstance().optimise();
}

bool DatabaseService::backup() noexcept
{
    const QString &logbookPath = getLogbookPath();
    QFileInfo logbookInfo = QFileInfo(logbookPath);

    const QString logbookDirectoryPath = logbookInfo.absolutePath();
    const QString baseName = logbookInfo.baseName();
    const QString backupDirectoryName = "Backups";
    QDir logbookDir(logbookDirectoryPath);
    bool ok;
    if (!logbookDir.exists(backupDirectoryName)) {
        ok = logbookDir.mkdir(backupDirectoryName);
    } else {
        ok = true;
    }
    if (ok) {
        const QString backupLogbookDirectoryPath = logbookDirectoryPath + "/" + backupDirectoryName;
        const QString baseBackupLogbookName = baseName + "-" + QDateTime::currentDateTime().toString("yyyy-MM-dd hhmm");
        QString backupLogbookName = baseBackupLogbookName + LogbookExtension;
        QDir backupLogbookDir(backupLogbookDirectoryPath);
        int index = 1;
        while (backupLogbookDir.exists(backupLogbookName) && index <= MaxBackupIndex) {
            backupLogbookName = baseBackupLogbookName + QString("-%1").arg(index) + LogbookExtension;
            ++index;
        }
        ok = index <= MaxBackupIndex;
        if (ok) {
            const QString backupLogbookPath = backupLogbookDirectoryPath + "/" + backupLogbookName;
            ok = ConnectionManager::getInstance().backup(backupLogbookPath);
        }
    }
    return ok;
}

bool DatabaseService::getMetadata(Metadata &metadata) noexcept
{
    return ConnectionManager::getInstance().getMetadata(metadata);
}
