#include "widgets/settings_dialog.h"
#include "ui_settings_dialog.h"

Settings_Dialog::Settings_Dialog(Settings *settings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings_Dialog)
    , m_settings(settings)
{
    ui->setupUi(this);
    ui->ddLeftClick->addItem(tr("Begin drag"), LeftClickAction::BeginDrag);
    ui->ddLeftClick->addItem(tr("Next / Previous Image"), LeftClickAction::ChangeImage);

    ui->ddMiddleClick->addItem(tr("<None>"),      MiddleClickAction::None);
    ui->ddMiddleClick->addItem(tr("Full Screen"),
                               MiddleClickAction::Fullscreen);
    ui->ddMiddleClick->addItem(tr("Autofit"),     MiddleClickAction::AutoFit);
    ui->ddMiddleClick->addItem(tr("Actual Size"), MiddleClickAction::ZoomReset);
    ui->ddMiddleClick->addItem(tr("Next Image"),  MiddleClickAction::NextImage);
    ui->ddMiddleClick->addItem(tr("Boss Key"),    MiddleClickAction::Boss);
    ui->ddMiddleClick->addItem(tr("Quit"),        MiddleClickAction::Quit);

    ui->ddMiddleClick->setCurrentIndex(
                ui->ddMiddleClick->findData(m_settings->getMiddleClick()));


    ui->ddWheel->addItem(tr("<None>"),                WheelAction::None);
    ui->ddWheel->addItem(tr("Scroll Image"),          WheelAction::Scroll);
    ui->ddWheel->addItem(tr("Next / Previous Image"), WheelAction::ChangeImage);
    ui->ddWheel->addItem(tr("Zoom In / Out"),         WheelAction::Zoom);

    ui->ddWheel->setCurrentIndex(ui->ddWheel->findData(m_settings->getWheel()));
    ui->cbRTL->setChecked(m_settings->getRightToLeft());
    ui->cbHardwareAcceleration->setChecked(
                m_settings->getHardwareAcceleration());
    ui->cbSmoothZoom->setChecked(
                m_settings->getZoomFilter() != ZoomFilter::None),
    ui->spinBox_thumbnail_width->setValue(
                m_settings->getThumbnailSize().width());
    ui->spinBox_thumbnail_height->setValue(
                m_settings->getThumbnailSize().height());

    ui->checkBox_calculate_average_color->setChecked(
                m_settings->getCalculateAverageColor());
}

void Settings_Dialog::on_buttonBox_accepted()
{
    m_settings->setLeftClick(
                ui->ddLeftClick->itemData(ui->ddLeftClick->currentIndex()).
                toInt());
    m_settings->setMiddleClick(
                ui->ddMiddleClick->itemData(ui->ddMiddleClick->currentIndex()).
                toInt());
    m_settings->setWheel(
                ui->ddWheel->itemData(ui->ddWheel->currentIndex()).toInt());
    m_settings->setRightToLeft(ui->cbRTL->isChecked());

    m_settings->setHardwareAcceleration(
                ui->cbHardwareAcceleration->isChecked());
    m_settings->setZoomFilter(
                ui->cbSmoothZoom->isChecked() ?
                    ZoomFilter::Good :
                    ZoomFilter::None);
    m_settings->setThumbnailSize(QSize(ui->spinBox_thumbnail_width->value(),
                                       ui->spinBox_thumbnail_height->value()));
    m_settings->setCalculateAverageColor(
                ui->checkBox_calculate_average_color->isChecked());
}

Settings_Dialog::~Settings_Dialog()
{
    delete ui;
}
