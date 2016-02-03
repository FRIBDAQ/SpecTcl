#ifndef SUBPROCESS_H
#define SUBPROCESS_H


/*!
 * \brief The Subprocess Interface
 *
 * Displays typically will cause a new process to be spawned and
 * managed. This interface provides some very basic functionality
 * expected to exist for all subprocesses.
 */
class Subprocess
{
    void exec() = 0;
    void kill() = 0;
    int getPid() const = 0;
    bool isRunning() const = 0;
};

#endif // SUBPROCESS_H
