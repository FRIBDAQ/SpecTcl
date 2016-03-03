#ifndef SPECTCLDISPLAYINTERFACE_H
#define SPECTCLDISPLAYINTERFACE_H

#include "DisplayInterface.h"

/*!
 * \brief A specialized display manager for spectcl
 *
 * The only difference between this and the CDisplayInterface is that
 * this is constructed non-empty. On construction, it adds creators for the
 * null, xamine, and spectra displays. It also constructs an instance
 * of the null display.
 */
class CSpecTclDisplayInterface : public CDisplayInterface
{

public:
    CSpecTclDisplayInterface();
};

#endif // SPECTCLDISPLAYINTERFACE_H
