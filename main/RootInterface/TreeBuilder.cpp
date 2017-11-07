/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  TreeBuilder.cpp
 *  @brief: Implement classes needed to build event trees from param names.
 */
#include "TreeBuilder.h"
#include <algorithm>
#include <stdexcept>
/*-----------------------------------------------------------------------------
 *    TreeItemBaseClass
 *
 */

/**
 * constructor
 *    @param pName - Name of the object (folder, item or base name of vector).
 */
TreeItemBaseClass::TreeItemBaseClass(const char* pName) : m_name(pName)
{}

/**
 * getName
 *   @return std::string - name of the item.
 */
std::string
TreeItemBaseClass::getName() const
{
    return m_name;
}
/*------------------------------------------------------------------------------
 *   TreeFolder
 */

/**
 * constructor
 *  @param pName - Name of the folder.
 *  @note m_fFree- is initialized true implying that the class will destroy
 *                 its contents, unless, freeStorage is called to set that
 *                 false.
 */
TreeFolder::TreeFolder(const char* pName) :
    TreeItemBaseClass(pName),
    m_fFree(true)
{}

/**
 * destructor - if m_fFree is true, all tree items contained are destroyed.
 */
TreeFolder::~TreeFolder()
{
    if (m_fFree) {
        std::for_each (
            m_contents.begin(), m_contents.end(),
            [](std::pair<std::string, TreeItemBaseClass*> p) {delete p.second;}
        );
    }
    // Note that destruction of m_contents will kill off the tree nodes.
}
/**
 * addItem
 *    @param pItem - Pointer to the item to add.  If m_fFree is true,
 *                   this must be dynamically allocated.
 *    @throws std::invalid_argument if an item with this name already exists.
 */
void
TreeFolder::addItem(TreeItemBaseClass* pItem)
{
    std::string name = pItem->getName();
    if (m_contents.count(name) > 0) {
        throw std::invalid_argument(
            "Attempting to insert a duplicate item in a tree directory"
        );
    }
    m_contents[name] = pItem;
}
/**
 * GetContents
 *    Gets immutable contents of the folder.
 * @return const Contents& - reference to m_contents.
 */
const TreeFolder::Contents&
TreeFolder::getContents() const
{
    return m_contents;
}
/**
 * begin
 *    @return const Contents::iterator - to the first item in the folder.
 */
TreeFolder::Contents::const_iterator
TreeFolder::begin() const
{
    Contents::const_iterator i = m_contents.begin();
    return i;
}
/**
 *  end
 *    @return const Contents::iterator - to off the end of the items in the folder.
 */
TreeFolder::Contents::const_iterator
TreeFolder::end() const
{
    Contents::const_iterator i =  m_contents.end();
    return i;
}
/**
 * size
 * @return size_t - number of items in the folder.
 */
size_t
TreeFolder::size() const
{
    return m_contents.size();
}
/**
 * freeStorage
 *    Controls whether or not the TreeItemBaseClass members of the
 *    folder are deleted on destruction
 * @param yesno - If true, the destructor will delete content members.
 */
void
TreeFolder::freeStorage(bool yesno)
{
    m_fFree = yesno;
}
/*------------------------------------------------------------------------------
 * TreeTerminal - a parameter.
 */

/**
 * constructor
 *   @param name - name of the parameter (tail of the path).
 *   @param parameterId - the parameter's id in the CEvent vector for the event.
 */
TreeTerminal::TreeTerminal(const char* name, unsigned parameterId) :
    TreeItemBaseClass(name),
    m_parameterId(parameterId) {}
    
/**
 * id
 *   @return unsigned - the parameter's id.
 */
unsigned
TreeTerminal::id() const
{
    return m_parameterId;
}
