#ifndef CALIBRATION_WIDGET_H
#define CALIBRATION_WIDGET_H

#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_calibration_widget.h"
#include "tools/calibration/calibration_intr.h"
#include "tools/calibration/chessboardcalibration.h"
#include "tools/calibration/charucocalibration.h"
#include "tools/imagecv.h"


namespace Ui {
class Calibration_widget;
}

enum DisplayerMode{
    ORIGINAL,
    CHESSBOARD_CORNERS,
    UNDISTORTED
};

class Calibration_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Calibration_widget(Calibration_intr*, QWidget *parent = nullptr);
    ~Calibration_widget();

    void display_image(DisplayerMode const&displayerMode);

private:
    Ui::Calibration_widget *ui;
    Calibration_intr* calib;
    DisplayerMode currentMode;


private slots:

    void on_undistortedButton_clicked();
    void on_chesscorners_clicked();
    void on_newImageSet_clicked();
    void on_nextImage_clicked();
    void on_prevImage_clicked();
    void on_save_clicked();
    void on_loadCalib_clicked();
    void on_originalImage_clicked();
    void on_calibration_clicked();
    void on_Calibration_widget_destroyed();
    void on_CharucoCalib_clicked();
};

#endif // CALIBRATION_WIDGET_H
