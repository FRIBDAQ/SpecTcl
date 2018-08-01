#ifndef DESIGNBYCONTRACT_H
#define DESIGNBYCONTRACT_H

#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>


namespace DesignByContract
{
    /////////////////////////////
    // General-purpose assertions
    /////////////////////////////

    class DesignByContractException : public std::runtime_error
    {
    protected:
        // Construction - only for derived classes

        DesignByContractException(const std::string& file,
                                  int                line,
                                  const std::string& what = std::string())
        : std::runtime_error(what),
          file_(file),
          line_(line),
          errorPrefix_("DesignByContract failed")
        {}
	~DesignByContractException() throw () {}
    public:
        // Queries

        // Exception error prefix
        std::string errorPrefix() const { return errorPrefix_; }

        // Source file
        std::string file() const { return file_; }

        // Line number
        int line() const { return line_; }

        // Description (from base)
        // virtual const char *what()

        // Source, location and description of exception
        operator std::string() const
        {
            std::ostringstream stream;
            stream << "File: " << file() << "\nLine: " << line() << "\n" << errorPrefix() << ". " << what() << "\n";
            return stream.str();
        }

    protected:
        // Commands
        void setErrorPrefix(const std::string& errorPrefix) { errorPrefix_ = errorPrefix; }

    private:
        // Implementation

        std::string file_;
        int         line_;
        std::string errorPrefix_;
    };

    // Source, location and description of exception
    inline
    std::ostream& operator<<(std::ostream& stream, const DesignByContractException& e)
    {
        return stream << "File: " << e.file() << "\nLine: " << e.line() << "\n" << e.errorPrefix() << ". " << e.what() << "\n";
    }

    class AssertionException : public DesignByContractException
    {
    public:
        AssertionException(const std::string& file,
                           int                line,
                           const std::string& what = std::string())
        : DesignByContractException(file, line, what)
        {
            setErrorPrefix("Assertion failed");
        }
    };

    typedef AssertionException Check;
    typedef AssertionException Assertion;

    ///////////////////////////////////////////////
    // Preconditions, Postconditions and Invariants
    ///////////////////////////////////////////////

    class PreconditionException : public DesignByContractException
    {
    public:
        PreconditionException(const std::string& file,
                              int                line,
                              const std::string& what = std::string())
        : DesignByContractException(file, line, what)
        {
            setErrorPrefix("Precondition failed");
        }
    };

    typedef PreconditionException Require;
    typedef PreconditionException Precondition;

    class PostconditionException : public DesignByContractException
    {
    public:
        PostconditionException(const std::string& file,
                               int                line,
                               const std::string& what = std::string())
        : DesignByContractException(file, line, what)
        {
            setErrorPrefix("Postcondition failed");
        }
    };

    typedef PostconditionException Ensure;
    typedef PostconditionException Postcondition;

    class InvariantException : public DesignByContractException
    {
    public:
        InvariantException(const std::string& file,
                           int                line,
                           const std::string& what = std::string())
        : DesignByContractException(file, line, what)
        {
            setErrorPrefix("Invariant failed");
        }
    };

    typedef InvariantException Invariant;

} // End namespace DesignByContract

// Conditional Compilation Constants

// DESIGN_BY_CONTRACT // Enable Design by Contract checks
// USE_ASSERTIONS     // Use ASSERT macros instead of exceptions
//
// These suggestions are based on Bertrand Meyer's Object-Oriented Software Construction (2nd Ed) p393
// http://www.eiffel.com/doc/manuals/technology/contract/
//
// CHECK_ALL           // Check assertions - implies checking preconditions, postconditions and invariants
// CHECK_INVARIANT     // Check invariants - implies checking preconditions and postconditions
// CHECK_POSTCONDITION // Check postconditions - implies checking preconditions
// CHECK_PRECONDITION  // Check preconditions only, e.g., in Release build
//
// So to enable all checks and use exception handling then, in a header file, write:
//
// #define DESIGN_BY_CONTRACT
// #define CHECK_ALL
//
// In your implementation file write:
//
// #include "mydefines.h"
// #include "DesignByContract.h"
// using namespace DesignByContract;
//
// Notes:
//
// In a derived class...
//
// 1. An overriding method may [only] weaken the precondition.
//    This means that the overriding precondition should be logically "or-ed" with the overridden precondition.
// 2. An overriding method may [only] strengthen the postcondition.
//    This means that the overriding postcondition should be logically "and-ed" with the overridden postcondition.
// 3. A derived class invariant should be logically "and-ed" with its base class invariant.

#if (defined(DESIGN_BY_CONTRACT) && !defined(USE_ASSERTIONS))

    #if defined(_DEBUG)

        #if defined(CHECK_ALL)

            #define REQUIRE0(assertion) \
                if (!(assertion)) throw Require(__FILE__, __LINE__)
            #define REQUIRE(assertion, what) \
                if (!(assertion)) throw Require(__FILE__, __LINE__, what)
            #define ENSURE0(assertion) \
                if (!(assertion)) throw Ensure(__FILE__, __LINE__)
            #define ENSURE(assertion, what) \
                if (!(assertion)) throw Ensure(__FILE__, __LINE__, what)
            #define INVARIANT0(assertion) \
                if (!(assertion)) throw Invariant(__FILE__, __LINE__)
            #define INVARIANT(assertion, what) \
                if (!(assertion)) throw Invariant(__FILE__, __LINE__, what)
            #define CHECK0(assertion) \
                if (!(assertion)) throw Check(__FILE__, __LINE__)
            #define CHECK(assertion, what) \
                if (!(assertion)) throw Check(__FILE__, __LINE__, what)

        #elif defined(CHECK_INVARIANT)

            #define REQUIRE0(assertion) \
                if (!(assertion)) throw Require(__FILE__, __LINE__)
            #define REQUIRE(assertion, what) \
                if (!(assertion)) throw Require(__FILE__, __LINE__, what)
            #define ENSURE0(assertion) \
                if (!(assertion)) throw Ensure(__FILE__, __LINE__)
            #define ENSURE(assertion, what) \
                if (!(assertion)) throw Ensure(__FILE__, __LINE__, what)
            #define INVARIANT0(assertion) \
                if (!(assertion)) throw Invariant(__FILE__, __LINE__)
            #define INVARIANT(assertion, what) \
                if (!(assertion)) throw Invariant(__FILE__, __LINE__, what)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #elif defined(CHECK_POSTCONDITION)

            #define REQUIRE0(assertion) \
                if (!(assertion)) throw Require(__FILE__, __LINE__)
            #define REQUIRE(assertion, what) \
                if (!(assertion)) throw Require(__FILE__, __LINE__, what)
            #define ENSURE0(assertion) \
                if (!(assertion)) throw Ensure(__FILE__, __LINE__)
            #define ENSURE(assertion, what) \
                if (!(assertion)) throw Ensure(__FILE__, __LINE__, what)
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #elif defined(CHECK_PRECONDITION)

            #define REQUIRE0(assertion) \
                if (!(assertion)) throw Require(__FILE__, __LINE__)
            #define REQUIRE(assertion, what) \
                if (!(assertion)) throw Require(__FILE__, __LINE__, what)
            #define ENSURE0(assertion)         ((void)0)
            #define ENSURE(assertion, what)    ((void)0)
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #else // Disable everything

            #define REQUIRE0(assertion)        ((void)0)
            #define REQUIRE(assertion, what)   ((void)0)
            #define ENSURE0(assertion)         ((void)0)
            #define ENSURE(assertion, what)    ((void)0)
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #endif // CHECK_ALL

    #else // Release

        // A suggested scheme is to enable preconditions only

        #if defined(CHECK_PRECONDITION)

            #define REQUIRE0(assertion) \
                if (!(assertion)) throw Require(__FILE__, __LINE__)
            #define REQUIRE(assertion, what) \
                if (!(assertion)) throw Require(__FILE__, __LINE__, what)
            #define ENSURE0(assertion)         ((void)0)
            #define ENSURE(assertion, what)    ((void)0)
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #else

            #define REQUIRE0(assertion)        ((void)0)
            #define REQUIRE(assertion, what)   ((void)0)
            #define ENSURE0(assertion)         ((void)0)
            #define ENSURE(assertion, what)    ((void)0)
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #endif

    #endif // _DEBUG

#elif (defined(DESIGN_BY_CONTRACT) && defined(USE_ASSERTIONS))

    #if defined(_DEBUG)

        #if defined(CHECK_ALL)

            #define REQUIRE(assertion, what) _ASSERTE(("Precondition failed." what, assertion))
            #define REQUIRE0(assertion) _ASSERTE(("Precondition failed.", assertion))
            #define ENSURE(assertion, what) _ASSERTE(("Postcondition failed." what, assertion))
            #define ENSURE0(assertion) _ASSERTE(("Postcondition failed.", assertion))
            #define INVARIANT(assertion, what) _ASSERTE(("Invariant failed." what, assertion))
            #define INVARIANT0(assertion) _ASSERTE(("Invariant failed.", assertion))
            #define CHECK(assertion, what) _ASSERTE(("Assertion failed." what, assertion))
            #define CHECK0(assertion) _ASSERTE(("Assertion failed.", assertion))

        #elif defined(CHECK_INVARIANT)

            #define REQUIRE(assertion, what) _ASSERTE(("Precondition failed." what, assertion))
            #define REQUIRE0(assertion) _ASSERTE(("Precondition failed.", assertion))
            #define ENSURE(assertion, what) _ASSERTE(("Postcondition failed." what, assertion))
            #define ENSURE0(assertion) _ASSERTE(("Postcondition failed.", assertion))
            #define INVARIANT(assertion, what) _ASSERTE(("Invariant failed." what, assertion))
            #define INVARIANT0(assertion) _ASSERTE(("Invariant failed.", assertion))
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #elif defined(CHECK_POSTCONDITION)

            #define REQUIRE(assertion, what) _ASSERTE(("Precondition failed." what, assertion))
            #define REQUIRE0(assertion) _ASSERTE(("Precondition failed.", assertion))
            #define ENSURE(assertion, what) _ASSERTE(("Postcondition failed." what, assertion))
            #define ENSURE0(assertion) _ASSERTE(("Postcondition failed.", assertion))
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #elif defined(CHECK_PRECONDITION)

            #define REQUIRE(assertion, what) _ASSERTE(("Precondition failed." what, assertion))
            #define REQUIRE0(assertion) _ASSERTE(("Precondition failed.", assertion))
            #define ENSURE0(assertion)         ((void)0)
            #define ENSURE(assertion, what)    ((void)0)
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #else // Disable everything

            #define REQUIRE0(assertion)        ((void)0)
            #define REQUIRE(assertion, what)   ((void)0)
            #define ENSURE0(assertion)         ((void)0)
            #define ENSURE(assertion, what)    ((void)0)
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

        #endif // CHECK_ALL

    #else // Release - no assertions possible

            #define REQUIRE0(assertion)        ((void)0)
            #define REQUIRE(assertion, what)   ((void)0)
            #define ENSURE0(assertion)         ((void)0)
            #define ENSURE(assertion, what)    ((void)0)
            #define INVARIANT0(assertion)      ((void)0)
            #define INVARIANT(assertion, what) ((void)0)
            #define CHECK0(assertion)          ((void)0)
            #define CHECK(assertion, what)     ((void)0)

    #endif // _DEBUG

#else // Not DESIGN_BY_CONTRACT - disable everything

    #define CHECK0(assertion)          ((void)0)
    #define CHECK(assertion, what)     ((void)0)
    #define REQUIRE0(assertion)        ((void)0)
    #define REQUIRE(assertion, what)   ((void)0)
    #define ENSURE0(assertion)         ((void)0)
    #define ENSURE(assertion, what)    ((void)0)
    #define INVARIANT0(assertion)      ((void)0)
    #define INVARIANT(assertion, what) ((void)0)

#endif // DESIGN_BY_CONTRACT

#endif //__DESIGNBYCONTRACT_H__
