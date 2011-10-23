#include "settings_dialog.h"
#include "ui_settings_dialog.h"

#include <QtGui/qpushbutton.h>
#include <QtGui/qvalidator.h>

Settings_Dialog::Settings_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings_Dialog)
{
    ui->setupUi(this);

    QIntValidator *iv = new QIntValidator();
    iv->setRange(0, 10000);
    ui->tbWaitTime->setValidator(iv);

    QHash<QString, MiddleClick::Action> ddMiddleClickHash;
    ddMiddleClickHash["<None>"] = MiddleClick::None;
    ddMiddleClickHash["Full Screen"] = MiddleClick::Fullscreen;
    ddMiddleClickHash["Autofit"] = MiddleClick::AutoFit;
    ddMiddleClickHash["Actual Size"] = MiddleClick::ZoomReset;
    ddMiddleClickHash["Follow Mouse"] = MiddleClick::FollowMouse;
    ddMiddleClickHash["Next Page"] = MiddleClick::NextPage;

    ddMidleClickOrder << MiddleClick::None
                      << MiddleClick::Fullscreen
                      << MiddleClick::AutoFit
                      << MiddleClick::ZoomReset
                      << MiddleClick::NextPage
//                      << MiddleClick::FollowMouse
                         ;

    for(int i=0; i < ddMidleClickOrder.count(); ++i)
    {
        ui->ddMiddleClick->addItem(ddMiddleClickHash.key(ddMidleClickOrder.at(i)));
        if (ddMidleClickOrder.at(i) == Settings::Instance()->getMiddleClick())
        {
            ui->ddMiddleClick->setCurrentIndex(i);
        }
    }


    QHash<QString, Wheel::Action> ddWheelHash;
    ddWheelHash["<None>"] = Wheel::None;
    ddWheelHash["Scroll Page"] = Wheel::Scroll;
    ddWheelHash["Next / Previous Page"] = Wheel::ChangePage;
    ddWheelHash["Zoom In / Out"] = Wheel::Zoom;

    ddWheelOrder << Wheel::None <<Wheel::Scroll << Wheel::ChangePage << Wheel::Zoom;

    for(int i=0; i < ddWheelOrder.count(); ++i)
    {
        ui->ddWheel->addItem(ddWheelHash.key(ddWheelOrder.at(i)));
        if (ddWheelOrder.at(i) == Settings::Instance()->getWheel())
        {
            ui->ddWheel->setCurrentIndex(i);
        }
    }

    ui->cbScrollByWidth->setChecked(Settings::Instance()->getScrollPageByWidth());
    ui->cbRTL->setChecked(Settings::Instance()->getRightToLeft());
    ui->tbWaitTime->setText(QString::number(Settings::Instance()->getPageChangeTimeout()));
    ui->cbJumpToEnd->setChecked(Settings::Instance()->getJumpToEnd());
    ui->cbHardwareAcceleration->setChecked(Settings::Instance()->getHardwareAcceleration());
}

void Settings_Dialog::on_buttonBox_accepted()
{
    Settings::Instance()->setMiddleClick(ddMidleClickOrder.at(ui->ddMiddleClick->currentIndex()));
    Settings::Instance()->setWheel(ddWheelOrder.at(ui->ddWheel->currentIndex()));

    Settings::Instance()->setScrollPageByWidth(ui->cbScrollByWidth->isChecked());
    Settings::Instance()->setRightToLeft(ui->cbRTL->isChecked());
    Settings::Instance()->setJumpToEnd(ui->cbJumpToEnd->isChecked());
    Settings::Instance()->setHardwareAcceleration(ui->cbHardwareAcceleration->isChecked());

    bool ok;
    int dec = ui->tbWaitTime->text().toInt(&ok, 10);
    if (ok)
    {
        Settings::Instance()->setPageChangeTimeout(dec);
    }
}

Settings_Dialog::~Settings_Dialog()
{
    delete ui;
}
