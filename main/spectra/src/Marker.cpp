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

/** @file:  Marker.cpp
 *  @brief: Implement the Viewer::Marker class.
 */

#include "Marker.h"
#include <TPad.h>
#include <TMarker.h>

static const int ROOT_CIRCULAR_MARKER=20;   // Marke type for filled circle.

namespace Viewer {
    
    /**
     * constructor
     *    @param x,y   - World coordinates of the marker point.
     *    @param name  - name of the marker.
     *
     */
    Marker::Marker(Double_t x, Double_t y, QString name) :
        GraphicalObject(name),
        m_pMarker(0)
    {
        m_pMarker = new TMarker(x, y, ROOT_CIRCULAR_MARKER);
        
        draw();
    }
    /**
     * destructor:
     *    destroy  the pad:
     */
    Marker::~Marker()
    {
        delete m_pMarker;
    }
    /**
     * move
     *    Redraw the marker elsewere
     *
     * @param x,y - new coordinates of the marker (WC)
     */
    void Marker::move(Double_t x, Double_t y)
    {
        m_pMarker->SetX(x);
        m_pMarker->SetY(y);
        draw();
        
    }
    /**
     * setX
     *    Set a new X coordinate for the marker.
     * @param x - new x
     */
    void Marker::setX(Double_t x)
    {
        m_pMarker->SetX(x);
        draw();
    }
    /**
     *  setY
     *    Set a new y coordinate for the marker.
     *
     * @param y - new y position
     */
    void Marker::setY(Double_t y)
    {
        m_pMarker->SetY(y);
        draw();
        
    }
    /**
     * Get marker coordinates:
     */
    Double_t Marker::getX() const {m_pMarker->GetX(); }
    Double_t Marker::getY() const {m_pMarker->GetY(); }
    
    
    ///////////////////////////////////////////////////////
    // Slot methods:
    
    void Marker::draw()
    {
        m_pMarker->Draw();
        gPad->SetLogx(gPad->GetLogx());   // empirically needed.
    }
    void Marker::draw(QRootCanvas* canvas)
    {
        // Markers can't change pads:
        
        draw();
    }
    
    //////////////// end namespace //////////////
}