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
#include <QString>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>

#include "../../../../../Kernel/src/Settings.h"
#include "../../../../../Model/src/AircraftType.h"
#include "../../../../../Persistence/src/Dao/AircraftTypeDaoIntf.h"
#include "../../../../../Persistence/src/Dao/DaoFactory.h"
#include "CSVImportDialog.h"
#include "ui_CSVImportDialog.h"

class CSVImportDialogPrivate
{
public:
    CSVImportDialogPrivate() noexcept
        : daoFactory(std::make_unique<DaoFactory>(DaoFactory::DbType::SQLite)),
          aircraftTypeDao(daoFactory->createAircraftTypeDao())
    {}

    std::unique_ptr<DaoFactory> daoFactory;
    std::unique_ptr<AircraftTypeDaoIntf> aircraftTypeDao;

    QPushButton *importButton;

};

// PUBLIC

CSVImportDialog::CSVImportDialog(QWidget *parent) noexcept
    : QDialog(parent),
      ui(new Ui::CSVImportDialog),
      d(std::make_unique<CSVImportDialogPrivate>())
{
    ui->setupUi(this);
    initUi();
    updateUi();
    frenchConnection();
}

CSVImportDialog::~CSVImportDialog() noexcept
{
    delete ui;
}

const QString CSVImportDialog::getSelectedFilePath() const noexcept
{
    return ui->filePathLineEdit->text();
}

bool CSVImportDialog::getSelectedAircraftType(AircraftType &aircraftType) const noexcept
{
    return d->aircraftTypeDao->getByType(ui->aircraftTypeComboBox->currentText(), aircraftType);
}

// PRIVATE

void CSVImportDialog::initUi() noexcept
{
    d->importButton = ui->buttonBox->addButton(tr("Import"), QDialogButtonBox::AcceptRole);
}

void CSVImportDialog::frenchConnection() noexcept
{
    connect(ui->filePathLineEdit, &QLineEdit::textChanged,
            this, &CSVImportDialog::updateUi);
}

// PRIVATE SLOTS

void CSVImportDialog::on_fileSelectionPushButton_clicked() noexcept
{
    // Start with the last export path
    QString exportPath = Settings::getInstance().getExportPath();

    const QString filePath = QFileDialog::getOpenFileName(this, QT_TRANSLATE_NOOP("CSVImportDialog", "Import CSV"), exportPath, QString("*.csv"));
    if (!filePath.isEmpty()) {
        ui->filePathLineEdit->setText(filePath);
    }
    updateUi();
}

void CSVImportDialog::updateUi() noexcept
{
    const QString filePath = ui->filePathLineEdit->text();
    QFile file(filePath);
    d->importButton->setEnabled(file.exists());
}
