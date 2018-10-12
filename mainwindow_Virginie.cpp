#include "mainwindow.h"

    MainWindow::MainWindow()
    {
        setFixedSize(1000,750);

        centralZone = new QWidget(this);
        setCentralWidget(centralZone);
        labelChosen = new QLabel(centralZone);

        /* FILE MENU*/
       fileMenu = new QMenu("File");
       menuBar()->addMenu(fileMenu);
       openAction = new QAction("Open",this);
       fileMenu-> addAction(openAction);
       connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));

       quitAction = new QAction("Quit", this);
       fileMenu -> addAction(quitAction);
       connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

       /*ABOUT MENU*/
       aboutMenu = new QMenu("About");
       menuBar() -> addMenu(aboutMenu);
       ourGroup = new QAction("Our group", this);
       aboutMenu->addAction(ourGroup);
       connect(ourGroup, SIGNAL(triggered(bool)), this, SLOT(openDialog()));
    }



 MainWindow::~MainWindow(){

 }
     void MainWindow::openImage(){

        QString imageUrl;
        imageUrl =QFileDialog::getOpenFileName(this, "Open", QString(), "Images (*.png *.gif *.jpg *.jpeg *.bmp)");

      /*Printing the image chosen in a QLabel*/

        QImage image(imageUrl);
        QPixmap pixmap = QPixmap::fromImage((image));
        labelChosen->setPixmap(pixmap);
        labelChosen->resize(image.size());
        labelChosen->show();

        /*Making conversions*/
        cv::Mat mat_img = QImage2cvmat(QImage(imageUrl));
       QImage q_image = cvmat2QImage(mat_img);
       /*Printing Cv:Mat image in a new Window*/
         //imshow("cv::Mat", mat_img);
        cv::Mat laplaceImage = contourLaplace(mat_img);
        cv::imshow("Laplace",laplaceImage);
        cv::Mat sobelImage = contourSobel(mat_img);
        cv::imshow("Sobel",sobelImage);
        /*Printing QImage in a new Window*/

        QLabel *labelImage = new QLabel(this);
        labelImage-> setWindowFlags(Qt::Window);
        labelImage->setWindowTitle("QImage after conversion");
        labelImage-> setPixmap(QPixmap::fromImage(q_image));
        labelImage->show();

    }


    void MainWindow::openDialog(){
        QMessageBox::information(this, "Our group", "Composition of our group :\n  \n- MONTALIBET Virginie \n \n- EUGENIE Meryl\n \n- MASSON Jordane");
    }



    cv::Mat MainWindow::QImage2cvmat(QImage const& image){
        QImage goodFormatImage= image.convertToFormat(QImage::Format_RGB888);
        QImage img = goodFormatImage.rgbSwapped();                                      //swapped R & B
        cv::Mat tempo = cv::Mat(img.height(), img.width(), CV_8UC3, (uchar*)img.bits(), img.bytesPerLine()); //tempo mat (byteperline --> alignement)
        cv::Mat imgReturned;
        cv::convertScaleAbs(tempo,imgReturned,1,0);
        return imgReturned;
    }



    QImage MainWindow:: cvmat2QImage(cv::Mat const& img){
        cv::Mat tempo;
        cv::cvtColor(img,tempo, cv::COLOR_BGR2RGB);                 //converting BGR to RGB
        QImage newimage = QImage((const uchar*)tempo.data, tempo.cols, tempo.rows, QImage::Format_RGB888); //construction of QImage
        newimage.bits();//good format (RGB)

        return newimage;
    }


    cv::Mat MainWindow::contourLaplace(cv::Mat img){
        cv::Mat gray_img, result, final;
        cv::Mat img_read = img.clone();

        cv::GaussianBlur(img_read,img_read,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
        cv::cvtColor(img_read,gray_img,CV_BGR2GRAY);

        cv::Laplacian(gray_img,result,CV_16S,3,1,0,cv::BORDER_DEFAULT);
        cv::convertScaleAbs(result,final,1,0);
        return final;
    }



    cv::Mat MainWindow::contourSobel(cv::Mat img){
        cv::Mat gray_img,final,gx,gy,gx_goodFormat, gy_goodFormat;

     // ENCODE & DECODE THE MAT INTO/FROM THE BUFFER (=IMWRITE)
        cv::Mat img_read=img.clone();

    // APPLY THE GAUSSIAN BLUR TO AVOID BLUR
        cv::GaussianBlur(img_read,img_read,cv::Size(3,3),0,0,cv::BORDER_DEFAULT);
        cv::cvtColor(img_read,gray_img,CV_BGR2GRAY); //CONVERT TO GRAY

        cv::Sobel(gray_img,gx,CV_16S,1,0,3,1,0,cv::BORDER_DEFAULT);  // DERIVATIVE IN X
        cv::Sobel(gray_img,gy,CV_16S,0,1,3,1,0,cv::BORDER_DEFAULT);// DERIVATIVE IN Y

        cv::convertScaleAbs(gy,gy_goodFormat,1,0);
        cv::convertScaleAbs(gx,gx_goodFormat,1,0);

        cv::addWeighted(gx_goodFormat,0.5,gy_goodFormat,0.5,0,final,-1); // FINAL GRADIENT = SUM OF X AND Y
        return final;

    }
