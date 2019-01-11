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

/** @file:  GraphicalObject.h
 *  @brief: Base class of all graphical objects.
 */

#ifndef VIEWER_GRAPHICALOBJECT_H
#define VIEWER_GRAPHICALOBJECT_H
#include <QObject>
#include <QString>

class QRootCanvas;

namespace Viewer {
    /**
     *  @class GraphicalObject
     *      This is the base class for all graphical objects.
     *      A graphical object is a purely local displayable object.
     *      This is an exact analog (well duplication) of Xamine grahpical
     *      objects and will include markers and summing regions.
     */
    class GraphicalObject : public QObject
    {
    private:
        Q_OBJECT
        QString      m_name;                  // Grobs have names.
    public:
        GraphicalObject(const QString& name);
        virtual ~GraphicalObject() {}         // Support virtual destruction.
        
        QString getName() const {return m_name; }
        
        // Things every graphical object must be able to do:
        
    public slots:
        virtual void draw() = 0;
        virtual void draw(QRootCanvas* canvas) = 0;
        
    
    };
}
#endif