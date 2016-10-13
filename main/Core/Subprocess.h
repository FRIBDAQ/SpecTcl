#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <string>

/*!
 * \brief The Subprocess Interface
 *
 * Displays typically will cause a new process to be spawned and
 * managed. This interface provides some very basic functionality
 * expected to exist for all subprocesses.
 */
class Subprocess
{
    virtual void exec() = 0;
    virtual void kill() = 0;
    virtual int getPid() const = 0;
    virtual bool isRunning() const = 0;
    virtual std::string generatePath() const = 0;
};

#endif // SUBPROCESS_H
