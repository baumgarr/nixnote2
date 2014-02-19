/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

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

#include "numberdelegate.h"

NumberDelegate::NumberDelegate(Format f)
{
    fmt = f;
}

QString NumberDelegate::displayText(const QVariant &value, const QLocale &locale) const {
    locale.name();  // Suppress unused variable
    // If we should hide zero values...
    if (value.toLongLong() == 0 && fmt == BlankNumber)
        return "";

    // If we should format in kb/mb
    if (fmt == KBNumber) {
       QString f = " B";
       float number = value.toLongLong();
       float value = number;
       if (number >= 1024 && number < 1048576) {
           value = number / 1024;
           f = " KB";
       } else {
           if (number >= 1048576) {
               f = " MB";
               value = number/1024/1024;
           }
       }
       QString rval =  QString().number(value);
       if (rval.indexOf(".") > 0)
          rval.truncate(rval.indexOf(".")+2);
       if (rval.endsWith(".0"))
           rval.truncate(rval.indexOf(".0"));
       return rval + f;
   }

   // Return default
   return value.toString();
}
