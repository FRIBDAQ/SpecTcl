/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  Marker.h
 *  @brief: Subclass of GRaphicalObject that provides markers.
 */

#ifndef VIEWER_MARKER_H
#define VIEWER_MARKER_H

#include "GraphicalObject.h"


namespace Viewer {
    /**
     * @class Viewer::Marker
     *
     * A marker is a histogram annotation.  It points out a spot on a spectrum.
     * For 1d spectra, this looks like a filled circle at a specific
     * channel/height.  For 2d spectra this looks like a filled circle at a
     * specific x/y channel pair.
     * 
     */
    class Marker : public GraphicalObject
    {
    private:
        Q_OBJECT                     // To participate in slots/signals.
        
        TMarker* m_pMarker;            // Underlying root marker.
        
    public:
        Marker(Double_t x, Double_t y, QString name);
        virtual ~Marker();
        
        void move(Double_t x, Double_t y);
        void setX(Double_t x);
        void setY(Double_t y);
        
        // Slots - these were virtual in the base class:
        
        virtual void draw();
        virtual void draw(QRootCanvas* canvas);
    };
}


#endif