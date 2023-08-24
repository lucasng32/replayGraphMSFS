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
#include <exception>
#include <filesystem>
#include <system_error>
#include <csignal>

#include <QtGlobal>
#include <QString>
#include <QStringBuilder>
#include <QMessageBox>

#include <Kernel/StackTrace.h>
#include <UserInterface/Dialog/TerminationDialog.h>
#include "ExceptionHandler.h"

namespace
{
    volatile std::sig_atomic_t signalStatus {0};
}

// PUBLIC

void ExceptionHandler::handle(const QString &title, const QString &stackTrace, const std::exception &ex) noexcept
{
    try {
        const QString exceptionMessage = exceptionToString(ex);
        handle(title, stackTrace, exceptionMessage);
    } catch (std::exception &ex) {
        // TODO IMPLEMENT ME Replace qDebug with qFatal (available as stream API from Qt 6.5 onwards)
        qDebug() << "Could not handle the original exception. Another std::exception occurred:" << ex.what();
    } catch (...) {
        qDebug() << "Could not handle the original exception. Another unknown exception occurred.";
    }
}

void ExceptionHandler::handle(const QString &title, const QString &stackTrace, const QString &exceptionMessage) noexcept
{
    try {
        qCritical() << "Exception message:" << exceptionMessage;
        TerminationDialog(title, exceptionMessage, stackTrace).exec();
    } catch (std::exception &ex) {
        qDebug() << "Could not handle the original exception. Another standard exception occurred:" << ex.what();
    } catch (...) {
        qDebug() << "Could not handle the original exception. Another unknown exception occurred.";
    }
}

void ExceptionHandler::handleTerminate() noexcept
{
    // Really make sure that we are not getting into an "endless termination loop"
    std::set_terminate(nullptr);

    // TODO Re-evaluate me: currently trying to generate a stack trace with cpptrace fails
    //      when run outside of a debugger and when in a std::terminate handler
    //      https://github.com/jeremy-rifkin/cpptrace/issues/27

    // const QString stackTrace = StackTrace::generate();
    QString stackTrace {"Stack trace not available in termination handler"};
    try {
        std::exception_ptr ex = std::current_exception();
        try {
            std::rethrow_exception(ex);
        } catch (std::exception &ex) {
            handle("Abnormal Termination", stackTrace, ex);
        } catch(...) {
            handle("Abnormal Termination", stackTrace, "Non std::exception");
        }
    } catch (std::exception &ex) {
        qDebug() << "Could not handle the errorneous program termination. Another standard exception occurred:" << ex.what();
    } catch (...) {
        qDebug() << "Could not handle the errorneous program termination. Another unknown (non-standard) exception occurred.";
    }

    std::abort();
}

void ExceptionHandler::signalHandler(int signal) noexcept
{
    ::signalStatus = signal;
}

int ExceptionHandler::getSignal() noexcept
{
    return ::signalStatus;
}

// PRIVATE

inline QString ExceptionHandler::errorCodeToString(const std::error_code &code)
{
    return QString("Error code: %1\nMessage: %2\nCategory: %3")
        .arg(code.value()).arg(code.message().c_str(), code.category().name());
}

QString ExceptionHandler::exceptionToString(const std::exception &ex)
{
    QString message;

    // File system
    auto baex = dynamic_cast<const std::bad_alloc *>(&ex);
    if (baex != nullptr) {
        message = QString("Memory could not be allocated: %1")
            .arg(baex->what());
        return message;
    }

    // File system
    auto fsex = dynamic_cast<const std::filesystem::filesystem_error *>(&ex);
    if (fsex != nullptr) {
        message = QString("A std::filesystem::filesystem_error:\n%1\npath 1: %2\npath 2: %3")
            .arg(fsex->what(), fsex->path1().c_str(), fsex->path2().c_str());
        if (fsex->code()) {
            message = message % '\n' % errorCodeToString(fsex->code());
        }
        return message;
    }

    // I/O
    auto iosex = dynamic_cast<const std::ios_base::failure *>(&ex);
    if (iosex != nullptr) {
        message = QString("A std::ios_base::failure occurred: %1")
            .arg(iosex->what());
        if (iosex->code()) {
            message = message % '\n' % errorCodeToString(iosex->code());
        }
        return message;
    }

    // Basic exception message
    message = QString("A std::exception occurred:\n%1").arg(ex.what());
    return message;
}
