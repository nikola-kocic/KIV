#include "settings_dialog.h"
#include "ui_settings_dialog.h"

Settings_Dialog::Settings_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings_Dialog)
{
    ui->setupUi(this);

    ui->ddMiddleClick->addItem(tr("<None>"),      MiddleClickAction::None);
    ui->ddMiddleClick->addItem(tr("Full Screen"), MiddleClickAction::Fullscreen);
    ui->ddMiddleClick->addItem(tr("Autofit"),     MiddleClickAction::AutoFit);
    ui->ddMiddleClick->addItem(tr("Actual Size"), MiddleClickAction::ZoomReset);
    ui->ddMiddleClick->addItem(tr("Next Page"),   MiddleClickAction::NextPage);
    ui->ddMiddleClick->addItem(tr("Boss Key"),    MiddleClickAction::Boss);
    ui->ddMiddleClick->addItem(tr("Quit"),        MiddleClickAction::Quit);

    ui->ddMiddleClick->setCurrentIndex(ui->ddMiddleClick->findData(Settings::Instance()->getMiddleClick()));


    ui->ddWheel->addItem(tr("<None>"),               WheelAction::None);
    ui->ddWheel->addItem(tr("Scroll Page"),          WheelAction::Scroll);
    ui->ddWheel->addItem(tr("Next / Previous Page"), WheelAction::ChangePage);
    ui->ddWheel->addItem(tr("Zoom In / Out"),        WheelAction::Zoom);

    ui->ddWheel->setCurrentIndex(ui->ddWheel->findData(Settings::Instance()->getWheel()));

    ui->cbScrollByWidth->setChecked(Settings::Instance()->getScrollPageByWidth());
    ui->cbRTL->setChecked(Settings::Instance()->getRightToLeft());
    ui->cbScrollChangesPage->setChecked(Settings::Instance()->getScrollChangesPage());
    ui->sbWaitTime->setValue(Settings::Instance()->getPageChangeTimeout());
    ui->cbJumpToEnd->setChecked(Settings::Instance()->getJumpToEnd());

    ui->cbHardwareAcceleration->setChecked(Settings::Instance()->getHardwareAcceleration());
    ui->sbThumbSize->setValue(Settings::Instance()->getThumbnailSize());

    ui->checkBox_calculate_average_color->setChecked(Settings::Instance()->getCalculateAverageColor());

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
    Settings::Instance()->setCalculateAverageColor(ui->checkBox_calculate_average_color->isChecked());
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
    if (ui->ddWheel->itemData(index).toInt() != WheelAction::Scroll)
    {
        ui->groupBox->setEnabled(false);
    }
    else
    {
        ui->groupBox->setEnabled(true);
    }
}
