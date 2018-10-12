#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
  MainWindow();
  ~MainWindow();
  cv::Mat QImage2cvmat( QImage const& image);
  QImage cvmat2QImage(cv::Mat const& img);
  cv::Mat contourLaplace(cv::Mat img);
  cv::Mat contourSobel(cv::Mat img);

private:
    QMenu *fileMenu;
    QMenu *aboutMenu ;
    QWidget *centralZone ;
    QAction *ourGroup;
    QAction *openAction;
    QAction *quitAction;
    QLabel *labelChosen;
    QString *imageUrl;


private slots :
void openImage();
void openDialog();
};
#endif // MAINWINDOW_H
