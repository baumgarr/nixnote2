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


#ifndef COMMUNICATIONERROR_H
#define COMMUNICATIONERROR_H

#include <QObject>


//************************************************
//* This class is used to let the user know
//* about any errors when doing a sync.
//************************************************

class CommunicationError : public QObject
{
    Q_OBJECT
public:

    // The type of errors possible
    enum CommunicationErrorType {
        None = 0,
        Unknown = 1,
        EDAMSystemException = 2,
        EDAMUserException = 3,
        TTransportException = 4,
        EDAMNotFoundException = 5,
        StdException = 6,
        TSSLException = 7,
        TException = 8,
        RateLimitExceeded = 9,
        ThriftException = 10
    };

    explicit CommunicationError(QObject *parent = 0);    // Constructor
    CommunicationErrorType type;  // The last error type we encountered
    QString message;              // The last error message from Evernote
    int code;                     // The EDAM error code
    void reset();                 // Reset all values
    bool retry();                 // Retry after the last error
    int retryCount;               // Current retry count
    int maxRetryCount;            // Maximum number of times to retry
    
signals:
    
public slots:
    
};

#endif // COMMUNICATIONERROR_H
