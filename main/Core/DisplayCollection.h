#ifndef DISPLAYCOLLECTION_H
#define DISPLAYCOLLECTION_H

#include <set>
#include <stdlib.h>

class CDisplay;

/*!
 * \brief The manager class for display
 *
 * The DisplayManager class is entirely responsible for maintaining
 * the list of displays that SpecTcl knows about. It does not own
 * any of the display instances that are registered to it. Instead it
 * is expected that some other entity will own each display. This will
 * just provide a list of viable displays that can be selected to be
 * the current display.
 */
class CDisplayCollection
{

private:
    typedef std::set<CDisplay*>            Container;
    typedef typename Container::iterator   Iterator;
    typedef typename Container::value_type ValueType;

    Container                m_displays;
    Container::iterator      m_current;

public:
    /*!
     * \brief Constructor
     *
     * The current display is set to an invalid value.
     */
    CDisplayCollection();

    /*!
     * \brief Check whether a display exists
     * \param pDisplay  the display to look for
     * \return boolean
     * \retval true     display was found
     * \retval false    display ws not found
     */
    bool displayExists(CDisplay* pDisplay);

    /*!
     * \brief Insert the display into the collection
     * \param pDisplay  display to add
     *
     * Note that if the display already exists, it is not an error to
     * add it again. The attempt to insert it the second time will cause
     * no change to the collection.
     */
    void addDisplay(CDisplay* pDisplay);

    /*!
     * \brief Remove a specific display from the collection
     * \param pDisplay  the display to remove
     *
     * If the display is not found in the collection, it is not an error.
     *
     * If the display being removed was the current canvas, the current
     * canvas is set to an invalid value.
     */
    void removeDisplay(CDisplay* pDisplay);


    /*!
     * \return CDisplay*
     * \retval nullptr  if no display has been set to current
     * \retval pointer to current display
     */
    CDisplay* getCurrentDisplay();

    /*!
     * \brief Set the current display
     * \param pDisplay  the display to become current
     *
     * If the display already exists in the collection, then it is selected.
     * If the display is not contained in the collection, it is added and then
     * set to current.
     *
     * \throw runtime_error is a nullptr is passed in.
     */
    void      setCurrentDisplay(CDisplay* pDisplay);

    /////////////////////////////////////////////////////////////////////
    // Iterator access

    /*!
     * \return iterator to first display
     */
    Iterator begin() { return m_displays.begin(); }

    /*!
     * \return iterator just beyond the last display
     */
    Iterator end()   { return m_displays.end(); }

    /*!
     * \return the number of displays in the collection
     */
    size_t size() const { return m_displays.size(); }
};


#endif // DISPLAYCOLLECTION_H
