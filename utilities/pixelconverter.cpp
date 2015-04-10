/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/


#include "pixelconverter.h"

PixelConverter::PixelConverter()
{
    pointArray.insert(8,6);
    pointArray.insert(9,7);
    pointArray.insert(10,8);   //Should really be 7.5
    pointArray.insert(11,8);
    pointArray.insert(12,9);
    pointArray.insert(13,10);
    pointArray.insert(14,11);  //Should really be 10.5
    pointArray.insert(15,11);
    pointArray.insert(16,12);
    pointArray.insert(17,13);
    pointArray.insert(18,14);  //should really be 14.5
    pointArray.insert(19,14);
    pointArray.insert(20,15);
    pointArray.insert(21,15);
    pointArray.insert(22,16);
    pointArray.insert(23,17);
    pointArray.insert(24,18);
    pointArray.insert(26,20);
    pointArray.insert(29,22);
    pointArray.insert(32,24);
    pointArray.insert(35,26);
    pointArray.insert(36,27);
    pointArray.insert(37,28);
    pointArray.insert(38,29);
    pointArray.insert(40,30);
    pointArray.insert(42,32);
    pointArray.insert(45,34);
    pointArray.insert(48,36);
}


int PixelConverter::getPoints(int px) {
    if (px <=0)
        return -1;
    if (pointArray.contains(px))
        return pointArray[px];
    return px*3/4;
}
