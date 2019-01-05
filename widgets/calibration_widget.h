#ifndef CALIBRATION_WIDGET_H
#define CALIBRATION_WIDGET_H

#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include "ui_calibration_widget.h"

#include "tools/calibration_intr.h"


namespace Ui {
class Calibration_widget;
}

class Calibration_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Calibration_widget(Calibration_intr*, QWidget *parent = nullptr);
    ~Calibration_widget();
    void show_chessboard_corners(cv::Mat*);
    void show_undistorted_image(cv::Mat*);
    void display_image(cv::Mat const&);

private:
    Ui::Calibration_widget *ui;
    Calibration_intr* calib;


private slots:

    void on_undistortedButton_clicked();
    void on_chesscorners_clicked();
};

#endif // CALIBRATION_WIDGET_H
