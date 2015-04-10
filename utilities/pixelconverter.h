#ifndef PIXELCONVERTER_H
#define PIXELCONVERTER_H

#include <QHash>

class PixelConverter
{
private:
    QHash<int,int> pointArray;
public:
    PixelConverter();
    int getPoints(int px);
};

#endif // PIXELCONVERTER_H
