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
    explicit Settings_Dialog(Settings *settings, QWidget *parent = nullptr);
    ~Settings_Dialog() override;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Settings_Dialog *ui;
    Settings *m_settings;
};

#endif  // SETTINGS_DIALOG_H
