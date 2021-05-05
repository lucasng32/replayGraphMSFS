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

#include <QDialog>

#include "../../../Model/src/SimVar.h"
#include "../../../Model/src/Logbook.h"
#include "../../../Model/src/Flight.h"
#include "../../../Model/src/Aircraft.h"
#include "../../../Model/src/AircraftData.h"
#include "../../../SkyConnect/src/SkyConnectIntf.h"
#include "../../../SkyConnect/src/Connect.h"
#include "../Unit.h"
#include "AircraftWidget.h"
#include "ui_AircraftWidget.h"

class AircraftWidgetPrivate
{
public:
    AircraftWidgetPrivate(const QWidget &widget, SkyConnectIntf &theSkyConnect) noexcept
        : skyConnect(theSkyConnect),
          ActiveTextColor(widget.palette().color(QPalette::Active, QPalette::WindowText)),
          DisabledTextColor(widget.palette().color(QPalette::Disabled, QPalette::WindowText))
    {}

    SkyConnectIntf &skyConnect;
    Unit unit;
    const QColor ActiveTextColor;
    const QColor DisabledTextColor;
};

AircraftWidget::AircraftWidget(SkyConnectIntf &skyConnect, QWidget *parent) noexcept :
    QWidget(parent),
    d(std::make_unique<AircraftWidgetPrivate>(*this, skyConnect)),
    ui(std::make_unique<Ui::AircraftWidget>())
{
    ui->setupUi(this);
    initUi();
}

AircraftWidget::~AircraftWidget() noexcept
{}

// PROTECTED

void AircraftWidget::showEvent(QShowEvent *event) noexcept
{
    Q_UNUSED(event)

    updateUi(d->skyConnect.getCurrentTimestamp(), TimeVariableData::Access::Seek);
    connect(&d->skyConnect, &SkyConnectIntf::timestampChanged,
            this, &AircraftWidget::updateUi);
}

void AircraftWidget::hideEvent(QHideEvent *event) noexcept
{
    Q_UNUSED(event)

    disconnect(&d->skyConnect, &SkyConnectIntf::timestampChanged,
            this, &AircraftWidget::updateUi);
}

// PRIVATE

void AircraftWidget::initUi() noexcept
{
    // Position
    ui->latitudeLineEdit->setToolTip(SimVar::Latitude);
    ui->longitudeLineEdit->setToolTip(SimVar::Longitude);
    ui->altitudeLineEdit->setToolTip(SimVar::Altitude);
    ui->pitchLineEdit->setToolTip(SimVar::Pitch);
    ui->bankLineEdit->setToolTip(SimVar::Bank);
    ui->headingLineEdit->setToolTip(SimVar::Heading);

    // Velocity
    ui->velocityXLineEdit->setToolTip(SimVar::VelocityBodyX);
    ui->velocityYLineEdit->setToolTip(SimVar::VelocityBodyY);
    ui->velocityZLineEdit->setToolTip(SimVar::VelocityBodyZ);
    ui->rotationVelocityXLineEdit->setToolTip(SimVar::RotationVelocityBodyX);
    ui->rotationVelocityYLineEdit->setToolTip(SimVar::RotationVelocityBodyY);
    ui->rotationVelocityZLineEdit->setToolTip(SimVar::RotationVelocityBodyZ);
}

const AircraftData &AircraftWidget::getCurrentAircraftData(qint64 timestamp, TimeVariableData::Access access) const noexcept
{
    const Aircraft &aircraft = Logbook::getInstance().getCurrentFlight().getUserAircraft();

    if (d->skyConnect.getState() == Connect::State::Recording) {
        return aircraft.getLast();
    } else {
        if (timestamp != TimeVariableData::InvalidTime) {
            return aircraft.interpolate(timestamp, access);
        } else {
            return aircraft.interpolate(d->skyConnect.getCurrentTimestamp(), access);
        }
    };
}

// PRIVATE SLOTS

void AircraftWidget::updateUi(qint64 timestamp, TimeVariableData::Access access) noexcept
{
    const AircraftData &aircraftData = getCurrentAircraftData(timestamp, access);
    QString colorName;

    if (!aircraftData.isNull()) {
        // Position
        ui->latitudeLineEdit->setText(d->unit.formatLatitude(aircraftData.latitude));
        ui->longitudeLineEdit->setText(d->unit.formatLongitude(aircraftData.longitude));
        ui->altitudeLineEdit->setText(d->unit.formatFeet(aircraftData.altitude));
        ui->pitchLineEdit->setText(d->unit.formatDegrees(aircraftData.pitch));
        ui->bankLineEdit->setText(d->unit.formatDegrees(aircraftData.bank));
        ui->headingLineEdit->setText(d->unit.formatDegrees(aircraftData.heading));

        // Velocity
        ui->velocityXLineEdit->setText(d->unit.formatVelocityInFeet(aircraftData.velocityBodyX));
        ui->velocityYLineEdit->setText(d->unit.formatVelocityInFeet(aircraftData.velocityBodyY));
        ui->velocityZLineEdit->setText(d->unit.formatVelocityInFeet(aircraftData.velocityBodyZ));
        ui->rotationVelocityXLineEdit->setText(d->unit.formatVelocityInRadians(aircraftData.rotationVelocityBodyX));
        ui->rotationVelocityYLineEdit->setText(d->unit.formatVelocityInRadians(aircraftData.rotationVelocityBodyY));
        ui->rotationVelocityZLineEdit->setText(d->unit.formatVelocityInRadians(aircraftData.rotationVelocityBodyZ));

        colorName = d->ActiveTextColor.name();
    } else {
        colorName = d->DisabledTextColor.name();
    }

    const QString css{QString("color: %1;").arg(colorName)};
    ui->latitudeLineEdit->setStyleSheet(css);
    ui->longitudeLineEdit->setStyleSheet(css);
    ui->altitudeLineEdit->setStyleSheet(css);
    ui->pitchLineEdit->setStyleSheet(css);
    ui->bankLineEdit->setStyleSheet(css);
    ui->headingLineEdit->setStyleSheet(css);
    ui->headingLineEdit->setStyleSheet(css);
    ui->velocityXLineEdit->setStyleSheet(css);
    ui->velocityYLineEdit->setStyleSheet(css);
    ui->velocityZLineEdit->setStyleSheet(css);
    ui->rotationVelocityXLineEdit->setStyleSheet(css);
    ui->rotationVelocityYLineEdit->setStyleSheet(css);
    ui->rotationVelocityZLineEdit->setStyleSheet(css);
}
