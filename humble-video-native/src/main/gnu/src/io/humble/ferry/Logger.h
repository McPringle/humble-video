/*******************************************************************************
 * Copyright (c) 2014, Andrew "Art" Clarke.  All rights reserved.
 *   
 * This file is part of Humble-Video.
 *
 * Humble-Video is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Humble-Video is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Humble-Video.  If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <cstdarg>

#ifdef VS_DEBUG
#include <assert.h>
#endif // VS_DEBUG

#include <io/humble/ferry/Ferry.h>
#include <io/humble/ferry/Mutex.h>
#include <io/humble/ferry/HumbleException.h>

namespace io { namespace humble { namespace ferry {
  /**
   * Internal Only
   * <p>
   * C++ wrapper to SLF4J Java Logging frame work.
   * </p>
   * <p>
   * If not running inside a JVM, then this class
   * just does a rudimentary printout of log messages
   * to stderr.
   * </p>
   * <p>
   * Otherwise, it forwards to Java's SLF4J logging framework.
   * </p>
   */
  class VS_API_FERRY Logger
  {
  public:
    /**
     * Different logging levels (noiseness) supported by us.
     */
    typedef enum Level
    {
      LEVEL_ERROR=0,
      LEVEL_WARN=1,
      LEVEL_INFO=2,
      LEVEL_DEBUG=3,
      LEVEL_TRACE=4
    } Level;

    /**
     * Returns a new Logger object for this loggerName.
     *
     * @param aLoggerName A name (no spaces allowed) for this logger.
     *
     * @return a New logger for logging; caller must delete when done.
     */
    static Logger* getLogger(const char *aLoggerName);

    /**
     * Get a Logger object, but ask the Logger code to
     * free it up once the JavaVM shuts down.  Use at your
     * own risk.
     *
     * @param aLoggerName A name (no spaces allowed) for this logger.
     *
     * @return A new logger for logging; caller must not call delete
     *  and must not use the logger once the JavaVM (or main) has exited.
     */
    static Logger* getStaticLogger(const char *aLoggerName);

    /**
     * Log the message to the logger, using sprintf() format
     * strings.
     *
     * @param filename The filename that is logging, or NULL.
     * @param lineNo The line number where this log statement is executed from.
     *  or 0.
     * @param level Level to log at.
     * @param format    A format specification string, in sprintf format.
     * @return if the message was actually logged.
     */
    bool log(const char* filename, int lineNo, Level level, const char *format, ...);
#ifndef SWIG // SWIG shouldn't know about this.
    bool logVA(const char* filename, int lineNo, Level level, const char *format, va_list ap);
#endif // SWIG
    /*
     * A series of convenience methods that model the slf4j
     * API
     */
    bool error(const char* filename, int lineNo,const char* format, ...);
    bool warn(const char* filename, int lineNo, const char* format, ...);
    bool info(const char* filename, int lineNo, const char* format, ...);
    bool debug(const char* filename, int lineNo, const char* format, ...);
    bool trace(const char* filename, int lineNo, const char* format, ...);


    bool isPrintStackTrace();
    void setPrintStackTrace(bool value);

    bool isLogging(Level level);
    void setIsLogging(Level level, bool value);
    static bool isGlobalLogging(Level level);
    static void setGlobalIsLogging(Level level, bool value);
    const char * getName();

    virtual ~Logger();

  protected:
    Logger(const char* loggerName, jobject aJavaLogger);
  private:
    static bool init();
    static void initJavaBindings(JavaVM* vm, void* closure);
    static bool mInitialized;
    static void shutdownJavaBindings(JavaVM* vm, void* closure);
    // This is the maximum size of a log message.  If
    // needed, someone should eventually reimplement this with
    // the right buffer-growth stuff, and with thread-safe
    // semantics, but for now, this implementation just allocates
    // this buffer on the stack before handing off to Java/stdio.
    static const int cMaxLogMessageLength=4096;
    static const int cMaxLoggerNameLength=255;
    char mLoggerName[cMaxLoggerNameLength+1];
    bool mIsLogging[5];
    bool mPrintStackTrace;

    bool doLog(Level level, const char*msg);
    bool doNativeLog(Level level, const char *msg);
    bool doJavaLog(Level level, const char* msg);

    jobject mJavaLogger;
    static jclass mClass;
    static jmethodID mGetLoggerMethod;
    static jmethodID mLogMethod;

    static Mutex *mClassMutex;
    static bool mGlobalIsLogging[5];
  };
}}}

/*
 * Now, for the wackly logging convenience macros
 */
#ifndef VS_CPP_PACKAGE
#define VS_CPP_PACKAGE native.__FILE
#endif // ! VS_CPP_PACKAGE

/*
 * To use this, put the following at the top of your CPP file:
 *
 * <code>
 *    VS_LOG_SETUP(VS_CPP_PACKAGE);
 * </code>
 *
 * Then, you can use any of these macros anywhere in the file to
 * log:
 *
 * <code>
 *      VS_LOG_ERROR("an example: %s", aStringVariable);
 * </code>
 *
 * IMPORTANT NOTE: These macros use "Variadic Macros" which are
 * not guaranteed to work on all compilers, but work on our
 * set of GCC 4.x, Visual Studio 2005+.
 * See http://en.wikipedia.org/wiki/Variadic_macro
 *
 */
#define VS_TOSTRING( S ) #S

#define VS_LOG_SETUP( LOGGERNAME ) \
  static io::humble::ferry::Logger* vs_logger_static_context( \
  io::humble::ferry::Logger::getStaticLogger( VS_TOSTRING( LOGGERNAME ) ) )

#define VS_LOG_ERROR(...) \
  (void) vs_logger_static_context->error(__FILE__, __LINE__, __VA_ARGS__)

#define VS_LOG_WARN(...) \
  (void) vs_logger_static_context->warn(__FILE__, __LINE__, __VA_ARGS__)

#define VS_LOG_INFO(...) \
  (void) vs_logger_static_context->info(__FILE__, __LINE__, __VA_ARGS__)

#define VS_LOG_DEBUG(...) \
  (void) vs_logger_static_context->debug(__FILE__, __LINE__, __VA_ARGS__)

#define VS_LOG_TRACE(...) \
  (void) vs_logger_static_context->trace(__FILE__, __LINE__, __VA_ARGS__)

#ifdef VS_DEBUG
#define VS_ASSERT( expr , msg ) \
  do { \
    if ( ! ( expr ) ) {\
      VS_LOG_ERROR("!!!ASSERTION FAILED!!! \"" VS_TOSTRING (expr) "\" with message \"%s\"", msg); \
    } \
    assert(expr); \
  } while (0)
#else
// don't insert anything if we're not debugging
#define VS_ASSERT( expr, msg ) (void) vs_logger_static_context;
#endif // VS_DEBUG

/**
 * Log the passed in error, and then throw it.
 */
#define VS_THROW(e) do { \
  const HumbleStackTrace& __humble_trace = (e); \
  const std::exception* __humble_err = dynamic_cast<const std::exception*>(&__humble_trace); \
  if (__humble_err) \
    VS_LOG_ERROR("%s: %s", VS_STRINGIFY(e), __humble_err->what()); \
  else \
    VS_LOG_ERROR("%s: unknown exception thrown", VS_STRINGIFY(e)); \
  int __humble_num_frames = __humble_trace.getNumFrames(); \
  if (vs_logger_static_context->isPrintStackTrace()) { for(int __humble_i = 0; __humble_i < __humble_num_frames; __humble_i++) { \
    const char* __humble_frame = __humble_trace.getFrameDescription(__humble_i); \
    VS_LOG_DEBUG("Stack frame [%d]: %s", __humble_i, __humble_frame); \
  } } \
  __humble_trace.raise(); \
} while (0)

/*
 *  Now, some logic to turn logging off on the compile line if needed.
 *
 * You can define the following on the compiler command line.  The
 * most conservative definition will win.
 *
 * VS_LOG_LOGLEVELS_NONE : no log messages get compiled in.
 * VS_LOG_LOGLEVELS_ERROR : only ERROR messages get compiled in.
 * VS_LOG_LOGLEVELS_WARN : only ERROR and WARN messages get compiled in.
 * VS_LOG_LOGLEVELS_INFO : ERROR, WARN and INFO messages get compiled in.
 * VS_LOG_LOGLEVELS_DEBUG: ERROR, WARN, INFO and Debug messages get compiled in.
 * VS_LOG_LEVLEVELS_ALL: All debug messages get compiled in.
 * none of these terms on CC line: All debug messages get compiled in.
 */
#ifdef VS_LOG_LOGLEVELS_NONE
#undef VS_LOG_ERROR
#undef VS_LOG_WARN
#undef VS_LOG_INFO
#undef VS_LOG_DEBUG
#undef VS_LOG_TRACE
#define VS_LOG_ERROR(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_WARN(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_INFO(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_DEBUG(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_TRACE(...) do { (void) vs_logger_static_context; } while(0)
#else
#ifdef VS_LOG_LOGLEVELS_ERROR
#undef VS_LOG_WARN
#undef VS_LOG_INFO
#undef VS_LOG_DEBUG
#undef VS_LOG_TRACE
#define VS_LOG_WARN(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_INFO(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_DEBUG(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_TRACE(...) do { (void) vs_logger_static_context; } while(0)
#else
#ifdef VS_LOG_LOGLEVELS_WARN
#undef VS_LOG_INFO
#undef VS_LOG_DEBUG
#undef VS_LOG_TRACE
#define VS_LOG_INFO(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_DEBUG(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_TRACE(...) do { (void) vs_logger_static_context; } while(0)
#else
#ifdef VS_LOG_LOGLEVELS_INFO
#undef VS_LOG_DEBUG
#undef VS_LOG_TRACE
#define VS_LOG_DEBUG(...) do { (void) vs_logger_static_context; } while(0)
#define VS_LOG_TRACE(...) do { (void) vs_logger_static_context; } while(0)
#else
#ifdef VS_LOG_LOGLEVELS_DEBUG
#undef VS_LOG_TRACE
#define VS_LOG_TRACE(...) do { (void) vs_logger_static_context; } while(0)
#else
#ifdef VS_LOG_LOGLEVELS_ALL
#else
#ifndef VS_DEBUG
#undef VS_LOG_TRACE
#define VS_LOG_TRACE(...) do { (void) vs_logger_static_context; } while(0)
#endif // ! VS_DEBUG
#endif // VS_LOG_LOGLEVELS_ALL
#endif // VS_LOG_LOGLEVELS_DEBUG
#endif // VS_LOG_LOGLEVELS_INFO
#endif // VS_LOG_LOGLEVELS_WARN
#endif // VS_LOG_LOGLEVELS_ERROR
#endif // VS_LOG_LOGLEVELS_NONE

#endif /*LOGGER_H_*/
