// Copyright (c) 2010, Razvan Petru
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef QSLOG_H
#define QSLOG_H

#include <QDebug>
#include <QString>
#include <typeinfo>

namespace QsLogging
{
class Destination;
enum Level
{
   TraceLevel = 0,
   DebugLevel,
   InfoLevel,
   WarnLevel,
   ErrorLevel,
   FatalLevel
};

class LoggerImpl; // d pointer
class Logger
{
public:
   static Logger& instance()
   {
      static Logger staticLog;
      return staticLog;
   }

   //! Adds a log message destination. Don't add null destinations.
   void addDestination(Destination* destination);
   //! Logging at a level < 'newLevel' will be ignored
   void setLoggingLevel(Level newLevel);
   //! The default level is INFO
   Level loggingLevel() const;

   //! The helper forwards the streaming to QDebug and builds the final
   //! log message.
   class Helper
   {
   public:
      explicit Helper(Level logLevel) :
            level(logLevel),
            qtDebug(&buffer) {}
      ~Helper();
      QDebug& stream(){ return qtDebug; }

   private:
      void writeToLog();

      Level level;
      QString buffer;
      QDebug qtDebug;
   };

private:
   Logger();
   Logger(const Logger&);
   Logger& operator=(const Logger&);
   ~Logger();

   void write(const QString& message);

   LoggerImpl* d;
};

} // end namespace


//! Logging macros: define QS_LOG_LINE_NUMBERS to get the file and line number
//! in the log output.
#define QS_LOG_LINE_NUMBERS 1
#ifndef QS_LOG_LINE_NUMBERS
   #define QLOG_TRACE() \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::TraceLevel ){} \
      else QsLogging::Logger::Helper(QsLogging::TraceLevel).stream()
   #define QLOG_DEBUG() \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::DebugLevel ){} \
      else QsLogging::Logger::Helper(QsLogging::DebugLevel).stream()
   #define QLOG_INFO()  \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::InfoLevel ){} \
      else QsLogging::Logger::Helper(QsLogging::InfoLevel).stream()
   #define QLOG_WARN()  \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::WarnLevel ){} \
      else QsLogging::Logger::Helper(QsLogging::WarnLevel).stream()
   #define QLOG_ERROR() \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::ErrorLevel ){} \
      else QsLogging::Logger::Helper(QsLogging::ErrorLevel).stream()
   #define QLOG_FATAL() \
      QsLogging::Logger::Helper(QsLogging::FatalLevel).stream()
#else
#define QLOG_TRACE_IN() \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::TraceLevel ){} \
      else (QsLogging::Logger::Helper(QsLogging::TraceLevel).stream() << '('<< __FILE__ << '@' << __LINE__ << ')' << "Entering" << __func__  << ":")
   #define QLOG_TRACE_OUT() \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::TraceLevel ){} \
      else (QsLogging::Logger::Helper(QsLogging::TraceLevel).stream() << '('<< __FILE__ << '@' << __LINE__ << ')' << "Exiting" << __func__ << ":")
   #define QLOG_TRACE() \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::TraceLevel ){} \
      else (QsLogging::Logger::Helper(QsLogging::TraceLevel).stream() << '('<< __FILE__ << '@' << __LINE__ << ')')
   #define QLOG_DEBUG() \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::DebugLevel ){} \
      else (QsLogging::Logger::Helper(QsLogging::DebugLevel).stream() << '('<< __FILE__ << '@' << __LINE__ << ')')
   #define QLOG_INFO()  \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::InfoLevel ){} \
      else (QsLogging::Logger::Helper(QsLogging::InfoLevel).stream() << '('<< __FILE__ << '@' << __LINE__ << ')')
   #define QLOG_WARN()  \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::WarnLevel ){} \
      else (QsLogging::Logger::Helper(QsLogging::WarnLevel).stream() << '('<< __FILE__ << '@' << __LINE__ << ')')
   #define QLOG_ERROR() \
      if( QsLogging::Logger::instance().loggingLevel() > QsLogging::ErrorLevel ){} \
      else (QsLogging::Logger::Helper(QsLogging::ErrorLevel).stream() << '('<< __FILE__ << '@' << __LINE__ << ')')
   #define QLOG_FATAL() \
      (QsLogging::Logger::Helper(QsLogging::FatalLevel).stream() << '('<< __FILE__ << '@' << __LINE__ << ')')
#endif

namespace qevercloud {
   template<typename T>
   class Optional;
}

template<typename T>
QDebug operator<<(QDebug dbg, const qevercloud::Optional<T> &opt)
{
   if (opt.isSet())
   {
      return dbg << opt.ref();
   }
   else
   {
      return dbg << "(unset)";
   }
}

#endif // QSLOG_H
