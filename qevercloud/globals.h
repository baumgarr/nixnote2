#ifndef QEVERCLOUD_GLOBALS_H
#define QEVERCLOUD_GLOBALS_H
#include <QDateTime>
#include <QNetworkAccessManager>
#include "exceptions.h"



//************************************************************
//* Customization for stack dump
//************************************************************
// The following include is needed for demangling names on a backtrace
#include <cxxabi.h>
#include <execinfo.h>
#include "logger/qslog.h"

//****************************
//* Ending customization
//****************************



/**
 * All the library lives in this namespace.
 */
namespace qevercloud {

/**
 * All network request made by QEverCloud - including OAuth - are
 * served by this NetworkAccessManager.
 *
 * Use this function to handle proxy authentication requests etc.
 */
QNetworkAccessManager* evernoteNetworkAccessManager();

/**
 * qevercloud library version.
 */
const int libraryVersion = 1*10000 + 1;


/**
 * Supports optional values.
 *
 * Most of the fields in the Evernote API structs are optional. But С++ does not support this notion directly.
 *
 * To implement the concept of optional values conventional Thrift C++ wrapper uses a special field of a struct type
 * where each field is of type bool with the same name as a field in the struct. This bool flag indicated was
 * the field with the same name in the outer struct assigned or not.
 *
 * While this method have its advantages (obviousness and simplicity) I found it very inconvenient to work with.
 * You have to check by hand that both values (value itself and its __isset flag) are in sync.
 * There is no checks whatsoever against an error and such an error is too easy to make.
 *
 * So for my library I created a special class that supports the optional value notion explicitly.
 * Basically Optional class just holds a bool value that tracks the fact that a value was assigned. But this tracking
 * is done automatically and attempts to use unissigned values throw exceptions. In this way errors are much harder to
 * make and it's harder for them to slip through testing unnoticed too.
 *
 */
template<typename T>
class Optional {
    bool isSet_;
    T value_;
public:
    /** Default constructor.
     * Default Optional is not set.
     */
    Optional(): isSet_(false), value_(T()) {}

    /**
     * Copy constructor.
     */
    Optional(const Optional& o): isSet_(o.isSet_), value_(o.value_) {}

    /**
     * Template copy constructor. Allows to be initialized with Optional of any compatible type.
     */
    template<typename X> Optional(const Optional<X>& o): isSet_(o.isSet_), value_(o.value_) {}

    /**
     * Initialization with a value of the type T. Note: it's implicit.
     */
    Optional(const T& value): isSet_(true), value_(value) {}

    /**
     * Template initialization with a value of any compatible type.
     */
    template<typename X> Optional(const X& value): isSet_(true), value_(value) {}

    /**
     * Assignment.
     */
    Optional& operator=(const Optional& o) {
        value_ = o.value_;
        isSet_ = o.isSet_;
        return *this;
    }

    /**
     * Template assignment with an Optional of any compatible value.
     */
    template<typename X> Optional& operator=(const Optional<X>& o) {
        value_ = o.value_;
        isSet_ = o.isSet_;
        return *this;
    }

    /**
     * Assignment with a value of the type T.
     */
    Optional& operator=(const T& value) {
        value_ = value;
        isSet_ = true;
        return *this;
    }

    /**
     * Template assignment with a value of any compatible type.
     */
    template<typename X> Optional& operator=(const X& value) {
        value_ = value;
        isSet_ = true;
        return *this;
    }

    /**
     * Implicit conversion of Optional<T> to T.
     *
     * const version.
     */
    operator const T&() const {
        if(!isSet_) throw EverCloudException("qevercloud::Optional: nonexistent value access1");
        return value_;
    }

    /**
     * Implicit conversion of Optional<T> to T.
     *
     * Note: a reference is returned, not a copy.
     */
    operator T&() {
        if(!isSet_) {

            //*********************************
            // Beginning customized dump
            //*********************************

            int max = 10;
            void *array[30];
            size_t size;
            QLOG_ERROR() << "***** Dumping stack *****";

            // get void*'s for all entries on the stack
            size = backtrace(array, 30);
            char **messages = backtrace_symbols(array, size);

            if (max > 0)
                size = max+1;  // We add one here because we always skip the first thing on the stack (this function).
            for (size_t i = 1; i < size && messages != NULL; ++i)
            {
                char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

                // find parantheses and +address offset surrounding mangled name
                for (char *p = messages[i]; *p; ++p)
                {
                    if (*p == '(')
                    {
                        mangled_name = p;
                    }
                    else if (*p == '+')
                    {
                        offset_begin = p;
                    }
                    else if (*p == ')')
                    {
                        offset_end = p;
                        break;
                    }
                }

                // if the line could be processed, attempt to demangle the symbol
                if (mangled_name && offset_begin && offset_end &&
                    mangled_name < offset_begin)
                {
                    *mangled_name++ = '\0';
                    *offset_begin++ = '\0';
                    *offset_end++ = '\0';

                    int status;
                    char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

                    // if demangling is successful, output the demangled function name
                    if (status == 0)
                    {
                       QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i] << " : "
                                  << real_name << "+" << offset_begin << offset_end;

                    }
                    // otherwise, output the mangled function name
                    else
                    {
                        QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i] << " : "
                                  << mangled_name << "+" << offset_begin << offset_end;
                    }
                    free(real_name);
                }
                // otherwise, print the whole line
                else
                {
                    QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i];
                }
            }

            free(messages);
            QLOG_ERROR() << "**** Stack dump complete *****";



            //*********************************
            // Ending customized dump
            //*********************************

            throw EverCloudException("qevercloud::Optional: nonexistent value access2");
        }
        return value_;
    }

    /**
     * Returs a reference to the holded value.
     *
     * const version.
     *
     */
    const T& ref() const {
        if(!isSet_) throw EverCloudException("qevercloud::Optional: nonexistent value access3");
        return value_;
    }

    /**
     * Returs reference to the holded value.
     *
     * There are contexts in C++ where impicit type conversions can't help. For example:
     *
     * @code
      Optional<QStringList> l;
      for(auto s : l); // you will hear from your compiler
      @endcode
     *
     * Explicit type conversion can be used...
     *
     * @code
      Optional<QStringList> l;
      for(auto s : static_cast<QStringList&>(l)); // ugh...
      @endcode
     *
     * ... but this is indeed ugly as hell.
     *
     * So I implemented ref() function that returns a reference to the holded value.
     * @code
      Optional<QStringList> l;
      for(auto s : l.ref()); // not ideal but OK
      @endcode
     */
    T& ref() {
        if(!isSet_) throw EverCloudException("qevercloud::Optional: nonexistent value access4");
        return value_;
    }

    /**
     * @brief Checks if value is set.
     * @return true if Optional have been assigned a value and false otherwise.
     *
     * Access to an unassigned ("not set") Optional lead to an exception.
     */
    bool isSet() const {return isSet_;}

    /**
     * Clears an Optional.
     *
     * @code

      Optional<int> o(1);
      o.clear();
      cout << o; // exception

      @endcode
     */
    void clear() {isSet_ = false; value_ = T();}

    /**
     * Fast way to initialize an Optional with a default value.
     *
     * It's very useful for structs.
     *
     * @code

      struct S2 {int f;};
      struct S {int f1; Optional<S2> f2};
      Optional<S> o; // o.isSet() != ture

      // without init() it's cumbersome to access struct fields
      // it's especially true for nested Optionals
      o = S(); // now o is set
      o->f2 = S2(); // and o.f2 is set
      o->f2->f = 1; // so at last it can be used

      // with init() it's simpler
      o.init()->f2.init()->f = 1;

      @endcode
     * @return reference to itself
     */
    Optional& init() {isSet_ = true; value_ = T(); return *this;}

    /**
     * Two syntatic constructs come to mind to use for implementation of access to a struct's/class's field directly from Optional.
     *
     * One is the dereference operator.
     * This is what boost::optional uses. While it's conceptually nice
     * I found it to be not a very convenient way to refer to structs, especially nested ones.
     * So I overloaded the operator-> and use smart pointer semantics.
     *
     * @code
      struct S1 {int f1;};
      struct S2 {Optional<S1> f2;};
      Optional<S2> o;

      *((*o).f2).f1; // boost way, not implemented

      o->f2->f1;     // QEverCloud way

      @endcode
     *
     * I admit, boost::optional is much more elegant overall. It uses pointer semantics quite clearly and
     * in an instantly understandable way. It's universal (* works for any type and not just structs). There is
     * no need for implicit type concersions and so there is no subtleties because of it. And so on.
     *
     * But then referring to struct fields is a chore. And this is the most common use case of Optionals in QEverCloud.
     *
     * So I decided to use non-obvious-on-the-first-sight semantics for my Optional. IMO it's much more convenient when gotten used to.
     *
     */
    T* operator->() {
        if(!isSet_) throw EverCloudException("qevercloud::Optional: nonexistent value access5");
        return &value_;
    }

    /**
     * const version.
     */
    const T* operator->() const {
        if(!isSet_) throw EverCloudException("qevercloud::Optional: nonexistent value access6");
        return &value_;
    }

    /**
     * The function is sometimes useful to simplify checking for the value being set.
     * @param defaultValue
     * The value to return if Optional is not set.
     * @return Optional value if set and defaultValue otherwise.
     */
    T value(T defaultValue = T()) const {
        return isSet_ ? value_ : defaultValue;
    }

    template<class X> friend class Optional;
};





}

#endif // QEVERCLOUD_GLOBALS_H
