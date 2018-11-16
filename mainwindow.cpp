#include <unistd.h>

#include <QtGui>
#include <QPixmap>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;
using namespace imagecv;

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
    load_image();
}

void MainWindow::on_button_orbs_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_image()){
            qDebug("[ERROR] No images loaded");
            return;
        }
    }

    split(*img_mat, img_left, img_right);
    Mat orb_Image = orb_features(*img_mat, img_left, img_right);
    QString str = speed_test((function_call_3_arg) orb_features, *img_mat, img_left, img_right);
    statusBar()->showMessage(str);
    imshow("orb Features", orb_Image);


}

void MainWindow::on_button_disparity_clicked(){
    parametersWindow->set_img_mat(img_mat);
    parametersWindow->show();
}

//Display Sobel Image
void MainWindow::on_button_sobel_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_image()){
            qDebug("[ERROR] No images loaded");
            return;
        }
    }
    cv::Mat img = contour_sobel(*img_mat);
    QImage image = mat_to_qimage(img);

    QString str = speed_test((function_call) contour_sobel, *img_mat);
    statusBar()->showMessage(str);

    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}

//Display Laplace image
void MainWindow::on_button_laplace_clicked(){
    if(img_mat->empty()){
        qDebug("[INFO] Load a stereo file before");
        if(!load_image()){
            qDebug("[ERROR] No images loaded");
            return;
        }
    }

    cv::Mat img = contour_laplace(*img_mat);
    QImage image = mat_to_qimage(img);

    QString str = speed_test((function_call) contour_laplace, *img_mat);
    statusBar()->showMessage(str);
    ui->backgroundLabel->setPixmap(QPixmap::fromImage(image));

}

bool MainWindow::load_image(){

    QString fileName = QFileDialog::getOpenFileName(this, tr("Sélectionnez une image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    QFile file(fileName);

    qDebug(" *** Loading file *** ");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {    //Check file validity/readable/...
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }
    if (!fileName.isEmpty()){
        if(load_file(fileName, img_mat)){
            QImage img_qimg = mat_to_qimage(*img_mat);         //Convert the new cv::mat to QImage
            ui->backgroundLabel->setPixmap(QPixmap::fromImage(img_qimg));    //Display the original image
            statusBar()->showMessage(tr("file loaded"), 2500);
            return true;
        }
    }
    return false;
}
