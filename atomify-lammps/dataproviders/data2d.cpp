#include "data2d.h"

Data2D::Data2D()
{
    int N = 256;

    QImage image(QSize(N,N), QImage::Format_RGB32);
//    for(int i=0; i<N; i++) {
//        for(int j=0; j<N; j++) {
//            QColor color;
//            color.setRed(i);
////            color.setGreen(i);
////            color.setBlue(i);
//            img.setPixel();
//            img.setPixelColor(i,j,color);
//        }
//    }

    for (int i = 0; i < image.height(); i++) {
        uchar* scan = image.scanLine(i);
        int depth =4;
        for (int j = 0; j < image.width(); j++) {
            QRgb* rgbpixel = reinterpret_cast<QRgb*>(scan + j*depth);
            int gray = i;
            *rgbpixel = QColor(gray, gray, gray).rgba();
        }
    }

    this->setMaxXValue(255);
    this->setMaxZValue(255);
    this->setHeightMap(image);
}
