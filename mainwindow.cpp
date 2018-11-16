#include <unistd.h>

#include <QtGui>
#include <QPixmap>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

/*  *********   */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    img_mat = new Mat;

    img_left = new Mat;
    img_right = new Mat;
    parametersWindow = new Disparity();

}

MainWindow::~MainWindow() {
    delete ui;


}


/**         Interface Interaction           **/

void MainWindow::on_actionA_propos_triggered() {
    QMessageBox::about(this, tr("About"),
             tr("Project members:\n\n"
                "Jordane Masson\n"
                "Virginie Montalibet\n"
                "Meryl Eugenie\n"));
}

void MainWindow::on_actionQuitter_triggered() {
    exit(EXIT_SUCCESS);
}

void MainWindow::on_actionOuvrir_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
}

void MainWindow::on_button_orbs_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                loadFile(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    imagecv::split(*img_mat, img_left, img_right);
    imagecv::orbFeatures(*img_mat);
    imagecv::disparityMapOrbs(*img_mat);

}

void MainWindow::on_button_disparity_clicked(){
    parametersWindow->setImg_mat(img_mat);
    parametersWindow->show();
}

//Display Sobel Image
void MainWindow::on_button_sobel_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                loadFile(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    cv::Mat img = imagecv::contourSobel(*img_mat);
    QImage image = imagecv::Mat2QImage(img);
    /*
     * TODO undefined ref error
    */
    float result = imagecv::speedTest(imagecv::contourSobel, * MainWindow::img_mat);
    QString str = "result of speed test: " + QString::number(result);
    statusBar()->showMessage(str);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}

//Display Laplace image
void MainWindow::on_button_laplace_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
            if (!fileName.isEmpty())
                loadFile(fileName);
    }

    if(img_mat->empty()){
        qDebug("[ERROR] No images loaded");
        return;
    }
    cv::Mat img = imagecv::contourLaplace(*img_mat);
    QImage image = imagecv::Mat2QImage(img);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}


/**         Others Functions           **/

bool MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);

    qDebug(" *** Loading file *** ");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {    //Check file validity/readable/...
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QImage myImage(fileName, "PNM");                                //load the file in  a QImage variable (pnm is a format like ttif, ...)
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(myImage));    //Display the original image

    qDebug(" *** Image file correctly loaded *** ");

    *img_mat = imagecv::QImage2Mat(myImage);         //Convert QImage to cv::mat
    QImage img_qimg = imagecv::Mat2QImage(*img_mat);         //Convert the new cv::mat to QImage

    statusBar()->showMessage(tr("file loaded"), 2500);

    return true;
}

