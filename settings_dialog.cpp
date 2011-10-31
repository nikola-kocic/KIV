#include "settings_dialog.h"
#include "ui_settings_dialog.h"

#include <QtGui/qpushbutton.h>
#include <QtGui/qvalidator.h>

Settings_Dialog::Settings_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings_Dialog)
{
    this->ui->setupUi(this);

    QHash<QString, MiddleClick::Action> ddMiddleClickHash;
    ddMiddleClickHash["<None>"]       = MiddleClick::None;
    ddMiddleClickHash["Full Screen"]  = MiddleClick::Fullscreen;
    ddMiddleClickHash["Autofit"]      = MiddleClick::AutoFit;
    ddMiddleClickHash["Actual Size"]  = MiddleClick::ZoomReset;
//    ddMiddleClickHash["Follow Mouse"] = MiddleClick::FollowMouse;
    ddMiddleClickHash["Next Page"]    = MiddleClick::NextPage;

    this->ddMidleClickOrder << MiddleClick::None
                            << MiddleClick::Fullscreen
                            << MiddleClick::AutoFit
                            << MiddleClick::ZoomReset
                            << MiddleClick::NextPage
//                          << MiddleClick::FollowMouse
                               ;

    for (int i = 0; i < this->ddMidleClickOrder.count(); ++i)
    {
        this->ui->ddMiddleClick->addItem(ddMiddleClickHash.key(this->ddMidleClickOrder.at(i)));
        if (this->ddMidleClickOrder.at(i) == Settings::Instance()->getMiddleClick())
        {
            this->ui->ddMiddleClick->setCurrentIndex(i);
        }
    }


    QHash<QString, Wheel::Action> ddWheelHash;
    ddWheelHash["<None>"]               = Wheel::None;
    ddWheelHash["Scroll Page"]          = Wheel::Scroll;
    ddWheelHash["Next / Previous Page"] = Wheel::ChangePage;
    ddWheelHash["Zoom In / Out"]        = Wheel::Zoom;

    this->ddWheelOrder << Wheel::None
                       << Wheel::Scroll
                       << Wheel::ChangePage
                       << Wheel::Zoom
                          ;

    for (int i = 0; i < this->ddWheelOrder.count(); ++i)
    {
        this->ui->ddWheel->addItem(ddWheelHash.key(this->ddWheelOrder.at(i)));
        if (this->ddWheelOrder.at(i) == Settings::Instance()->getWheel())
        {
            this->ui->ddWheel->setCurrentIndex(i);
        }
    }

    this->ui->cbScrollByWidth->setChecked(Settings::Instance()->getScrollPageByWidth());
    this->ui->cbRTL->setChecked(Settings::Instance()->getRightToLeft());
    this->ui->cbScrollChangesPage->setChecked(Settings::Instance()->getScrollChangesPage());
    this->ui->sbWaitTime->setValue(Settings::Instance()->getPageChangeTimeout());
    this->ui->cbJumpToEnd->setChecked(Settings::Instance()->getJumpToEnd());

    this->ui->cbHardwareAcceleration->setChecked(Settings::Instance()->getHardwareAcceleration());
    this->ui->sbThumbSize->setValue(Settings::Instance()->getThumbnailSize());

    on_cbScrollChangesPage_clicked(this->ui->cbScrollChangesPage->isChecked());
}

void Settings_Dialog::on_buttonBox_accepted()
{
    Settings::Instance()->setMiddleClick(this->ddMidleClickOrder.at(this->ui->ddMiddleClick->currentIndex()));
    Settings::Instance()->setWheel(this->ddWheelOrder.at(this->ui->ddWheel->currentIndex()));

    Settings::Instance()->setScrollPageByWidth(this->ui->cbScrollByWidth->isChecked());
    Settings::Instance()->setRightToLeft(this->ui->cbRTL->isChecked());
    Settings::Instance()->setScrollChangesPage(this->ui->cbScrollChangesPage->isChecked());
    Settings::Instance()->setJumpToEnd(this->ui->cbJumpToEnd->isChecked());
    Settings::Instance()->setPageChangeTimeout(this->ui->sbWaitTime->value());

    Settings::Instance()->setHardwareAcceleration(this->ui->cbHardwareAcceleration->isChecked());
    Settings::Instance()->setThumbnailSize(this->ui->sbThumbSize->value());
}

Settings_Dialog::~Settings_Dialog()
{
    delete this->ui;
}

void Settings_Dialog::on_cbScrollChangesPage_clicked(bool checked)
{
    this->ui->sbWaitTime->setEnabled(checked);
    this->ui->labelWaitTime->setEnabled(checked);
    this->ui->cbJumpToEnd->setEnabled(checked);
}
