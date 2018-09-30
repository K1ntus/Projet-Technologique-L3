#include <QtGui>
#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

cv::Mat QImage2Mat(QImage const& src);
QImage Mat2QImage(cv::Mat const& src);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionA_propos_triggered() {
    QMessageBox::about(this, tr("A propos"),
             tr("Project members:\n\n"
                "Jordane Masson\n"
                "Virginie Montalibet\n"
                "Meryl Eugenie\n"));
}

void MainWindow::on_actionQuitter_triggered()   {
    exit(EXIT_SUCCESS);
}

void MainWindow::on_actionOuvrir_triggered() {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
}

void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);

    qDebug(" *** * *** ");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QByteArray array = file.readAll();
    unsigned char* Data = (unsigned char*)&array.data()[0];
    QImage myImage(Data, this->height(),this->width(), QImage::Format_RGB888);
    //ui->backgroundLabel->setPixmap(QPixmap::fromImage(myImage));
    //waitKey(5000);

    cv::Mat img_mat = QImage2Mat(myImage);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(myImage));

    myImage = Mat2QImage(img_mat);
    //ui->backgroundLabel->setPixmap(QPixmap::fromImage(myImage));


    statusBar()->showMessage(tr("Fichier charge"), 2000);



}

QImage Mat2QImage(cv::Mat const& src){
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

cv::Mat QImage2Mat(QImage const& src){
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
     cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}
