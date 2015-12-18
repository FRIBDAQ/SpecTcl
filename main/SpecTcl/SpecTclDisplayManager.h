#ifndef SPECTCLDISPLAYINTERFACE_H
#define SPECTCLDISPLAYINTERFACE_H

#include "DisplayInterface.h"

/*!
 * \brief A specialized display manager for spectcl
 *
 * The only difference between this and the CDisplayManager is that
 * this is constructed non-empty. On construction, it adds an instance
 * of CNullDisplay.
 */
class CSpecTclDisplayInterface : public CDisplayInterface
{

public:
    CSpecTclDisplayInterface();
};

#endif // SPECTCLDISPLAYINTERFACE_H
