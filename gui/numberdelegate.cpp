#include "numberdelegate.h"

NumberDelegate::NumberDelegate(Format f)
{
    fmt = f;
}

QString NumberDelegate::displayText(const QVariant &value, const QLocale &locale) const {
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
