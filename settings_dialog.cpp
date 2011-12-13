#include "settings_dialog.h"
#include "ui_settings_dialog.h"

#include <QtGui/qpushbutton.h>
#include <QtGui/qvalidator.h>

Settings_Dialog::Settings_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings_Dialog)
{
    ui->setupUi(this);

    ui->ddMiddleClick->addItem(tr("<None>"),      MiddleClick::None);
    ui->ddMiddleClick->addItem(tr("Full Screen"), MiddleClick::Fullscreen);
    ui->ddMiddleClick->addItem(tr("Autofit"),     MiddleClick::AutoFit);
    ui->ddMiddleClick->addItem(tr("Actual Size"), MiddleClick::ZoomReset);
    ui->ddMiddleClick->addItem(tr("Next Page"),   MiddleClick::NextPage);
    ui->ddMiddleClick->addItem(tr("Boss Key"),    MiddleClick::Boss);
    ui->ddMiddleClick->addItem(tr("Quit"),       MiddleClick::Quit);

    ui->ddMiddleClick->setCurrentIndex(ui->ddMiddleClick->findData(Settings::Instance()->getMiddleClick()));


    ui->ddWheel->addItem(tr("<None>"),               Wheel::None);
    ui->ddWheel->addItem(tr("Scroll Page"),          Wheel::Scroll);
    ui->ddWheel->addItem(tr("Next / Previous Page"), Wheel::ChangePage);
    ui->ddWheel->addItem(tr("Zoom In / Out"),        Wheel::Zoom);

    ui->ddWheel->setCurrentIndex(ui->ddWheel->findData(Settings::Instance()->getWheel()));

    ui->cbScrollByWidth->setChecked(Settings::Instance()->getScrollPageByWidth());
    ui->cbRTL->setChecked(Settings::Instance()->getRightToLeft());
    ui->cbScrollChangesPage->setChecked(Settings::Instance()->getScrollChangesPage());
    ui->sbWaitTime->setValue(Settings::Instance()->getPageChangeTimeout());
    ui->cbJumpToEnd->setChecked(Settings::Instance()->getJumpToEnd());

    ui->cbHardwareAcceleration->setChecked(Settings::Instance()->getHardwareAcceleration());
    ui->sbThumbSize->setValue(Settings::Instance()->getThumbnailSize());

    on_cbScrollChangesPage_clicked(ui->cbScrollChangesPage->isChecked());
}

void Settings_Dialog::on_buttonBox_accepted()
{
    Settings::Instance()->setMiddleClick(ui->ddMiddleClick->itemData(ui->ddMiddleClick->currentIndex()).toInt());
    Settings::Instance()->setWheel(ui->ddWheel->itemData(ui->ddWheel->currentIndex()).toInt());

    Settings::Instance()->setScrollPageByWidth(ui->cbScrollByWidth->isChecked());
    Settings::Instance()->setRightToLeft(ui->cbRTL->isChecked());
    Settings::Instance()->setScrollChangesPage(ui->cbScrollChangesPage->isChecked());
    Settings::Instance()->setJumpToEnd(ui->cbJumpToEnd->isChecked());
    Settings::Instance()->setPageChangeTimeout(ui->sbWaitTime->value());

    Settings::Instance()->setHardwareAcceleration(ui->cbHardwareAcceleration->isChecked());
    Settings::Instance()->setThumbnailSize(ui->sbThumbSize->value());
}

Settings_Dialog::~Settings_Dialog()
{
    delete ui;
}

void Settings_Dialog::on_cbScrollChangesPage_clicked(bool checked)
{
    ui->sbWaitTime->setEnabled(checked);
    ui->labelWaitTime->setEnabled(checked);
    ui->cbJumpToEnd->setEnabled(checked);
}


void Settings_Dialog::on_ddWheel_currentIndexChanged(int index)
{
    if (ui->ddWheel->itemData(index).toInt() != Wheel::Scroll)
    {
        ui->groupBox->setEnabled(false);
    }
    else
    {
        ui->groupBox->setEnabled(true);
    }
}
