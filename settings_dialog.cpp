#include "settings_dialog.h"
#include "ui_settings_dialog.h"

Settings_Dialog::Settings_Dialog(Settings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings_Dialog)
{
    ui->setupUi(this);

    m_settings = settings;

    ui->ddMiddleClick->addItem(tr("<None>"),      MiddleClickAction::None);
    ui->ddMiddleClick->addItem(tr("Full Screen"), MiddleClickAction::Fullscreen);
    ui->ddMiddleClick->addItem(tr("Autofit"),     MiddleClickAction::AutoFit);
    ui->ddMiddleClick->addItem(tr("Actual Size"), MiddleClickAction::ZoomReset);
    ui->ddMiddleClick->addItem(tr("Next Page"),   MiddleClickAction::NextPage);
    ui->ddMiddleClick->addItem(tr("Boss Key"),    MiddleClickAction::Boss);
    ui->ddMiddleClick->addItem(tr("Quit"),        MiddleClickAction::Quit);

    ui->ddMiddleClick->setCurrentIndex(ui->ddMiddleClick->findData(m_settings->getMiddleClick()));


    ui->ddWheel->addItem(tr("<None>"),               WheelAction::None);
    ui->ddWheel->addItem(tr("Scroll Page"),          WheelAction::Scroll);
    ui->ddWheel->addItem(tr("Next / Previous Page"), WheelAction::ChangePage);
    ui->ddWheel->addItem(tr("Zoom In / Out"),        WheelAction::Zoom);

    ui->ddWheel->setCurrentIndex(ui->ddWheel->findData(m_settings->getWheel()));

    ui->cbScrollByWidth->setChecked(m_settings->getScrollPageByWidth());
    ui->cbRTL->setChecked(m_settings->getRightToLeft());
    ui->cbScrollChangesPage->setChecked(m_settings->getScrollChangesPage());
    ui->sbWaitTime->setValue(m_settings->getPageChangeTimeout());
    ui->cbJumpToEnd->setChecked(m_settings->getJumpToEnd());

    ui->cbHardwareAcceleration->setChecked(m_settings->getHardwareAcceleration());
    ui->sbThumbSize->setValue(m_settings->getThumbnailSize());

    ui->checkBox_calculate_average_color->setChecked(m_settings->getCalculateAverageColor());

    on_cbScrollChangesPage_clicked(ui->cbScrollChangesPage->isChecked());
}

void Settings_Dialog::on_buttonBox_accepted()
{
    m_settings->setMiddleClick(ui->ddMiddleClick->itemData(ui->ddMiddleClick->currentIndex()).toInt());
    m_settings->setWheel(ui->ddWheel->itemData(ui->ddWheel->currentIndex()).toInt());

    m_settings->setScrollPageByWidth(ui->cbScrollByWidth->isChecked());
    m_settings->setRightToLeft(ui->cbRTL->isChecked());
    m_settings->setScrollChangesPage(ui->cbScrollChangesPage->isChecked());
    m_settings->setJumpToEnd(ui->cbJumpToEnd->isChecked());
    m_settings->setPageChangeTimeout(ui->sbWaitTime->value());

    m_settings->setHardwareAcceleration(ui->cbHardwareAcceleration->isChecked());
    m_settings->setThumbnailSize(ui->sbThumbSize->value());
    m_settings->setCalculateAverageColor(ui->checkBox_calculate_average_color->isChecked());
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
