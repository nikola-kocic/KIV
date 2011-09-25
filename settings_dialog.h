#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include "settings.h"

namespace Ui {
    class Settings_Dialog;
}

class Settings_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Settings_Dialog(QWidget *parent = 0);
    ~Settings_Dialog();

private:
    Ui::Settings_Dialog *ui;

    QVector<Wheel::Action> ddWheelOrder;
    QVector<MiddleClick::Action> ddMidleClickOrder;
};

#endif // SETTINGS_DIALOG_H
