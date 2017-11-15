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
#include <cstring>
#include <Event.h>
#include <cmath>
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

/*----------------------------------------------------------------------------
 *  ParameterTree - the full parsed parameter tree.
 */

/**
 * Default constructor.
 *     The user is eventually expected to invoke buildTree to build the
 *     parameter tree.
 */
ParameterTree::ParameterTree() :
    TreeFolder("") {}

/**
 * constructor
 *    @param params  -Vector of parameter definitions that will be  used
 *                    to construct the tree.
 */
ParameterTree::ParameterTree(const std::vector<ParameterDef>& params) :
    TreeFolder("")
{
    buildTree(params);    
}
/**
 * destructor
 *    Clears the tree
 */
ParameterTree::~ParameterTree()
{
    clearTree();
}

/**
 * buildTree
 *     - Clears any existing parameter tree and
 *     - Constructs a new one, given the parameter definitions.
 * @param params - Vector of parameter definitions.
 */
void
ParameterTree::buildTree(const std::vector<ParameterDef>& params)
{
    clearTree();
    for (size_t i = 0; i < params.size(); i++) {
        addParameter(params[i]);
    }
}
/**
 * clearTree - clears the parameter tree currently stored.  See
 *              clearSubTree for more.
 */
void
ParameterTree::clearTree()
{
    clearSubTree(*this);
}

/*------------------ Utility methods for ParameterTree -----------------------*/

/**
 *   Adds a new parameter to the tree
 *   - Splits the name into path elements.
 *   - If necessary makes the full folder path down to the owning folder.
 *   - Adds the item to the folder.
 *
 * @param param - definition of the parameter.
*/
void
ParameterTree::addParameter(const ParameterDef& param)
{
    std::vector<std::string> path = pathElements(param.s_name.c_str());
    std::string tail = path.back();
    unsigned      id = param.s_id;
    path.pop_back();
    
    // Path is only the directory part of the path, while tail is the parameter name.
    
    TreeFolder* container = makeFolderPath(path);
    container->addItem(new TreeTerminal(tail.c_str(), id));
}
/**
 * makeFolderPath
 *    Given a directory path set, creates, as needed the set of folders
 *    required to make that path exist.
 *
 *  @param path - the folder path from top to bottom.
 *  @return TreeFolder* - Pointer to the folder at the end of the path.
 *  @throw std::invalid_argument - If a path element exists but is not a folder.
 */
TreeFolder*
ParameterTree::makeFolderPath(const std::vector<std::string>& path)
{
    // If there's no path elements this is a top folder:
    
    TreeFolder* result = this;   // if empty path this is the result.
    
    for (size_t i = 0; i < path.size(); i++) {
        
        // If needed, create the new tree folder:
        
        if (result->getContents().count(path[i]) == 0) {
            result->addItem(new TreeFolder(path[i].c_str()));   
        }
        TreeItemBaseClass* pItem = result->m_contents[path[i]];
        if (!pItem->isFolder()) {
            throw std::invalid_argument("Making folder path - an element exists but is not a folder");
        }
        result = reinterpret_cast<TreeFolder*>(pItem);
        
        // Go to the next level of the path.
    }
    
    return result;
}
/**
 * pathElements
 *    Takes a string with periods as path separators and splits the path
 *    into its consituent elements.
 *
 *  @param name - the path to split.
 *  @return std::vector<std::string> path elements, one per vector element.
 */
std::vector<std::string>
ParameterTree::pathElements(const char* name)
{
    std::vector<std::string> result;
    const char* pEnd = name + std::strlen(name);     // Pointer to the  end.
    while (true) {
        size_t n = std::strcspn(name, ".");          // Number of chars to delimeter.
        std::string element(name, n);
        result.push_back(element);
        if (n < strlen(name)) {
            name += n + 1;                       // Point past the .
        } else {
            break;                               // done.
        }
    }
    
    return result;
}
/**
 * clearSubTree
 *    Clear the folder hierarchy starting with but not including the folder
 *    given:
 *       For each item in the folder:
 *       -   If it is not a subfolder just delete it.
 *       -   If it is a subfolder, invoke clearSubTree on it, then delete it.
 *       -   Remove clear the contents of the folder.
 * @param top - Folder to empty out.
 */
void
ParameterTree::clearSubTree(TreeFolder& top)
{
    for (auto p = top.m_contents.begin(); p != top.m_contents.end(); p++) {
        if (p->second->isFolder()) {
            clearSubTree(*reinterpret_cast<TreeFolder*>(p->second));           // Recurse.
        }
        delete p->second;                       // Destroy the object.
    }
    top.m_contents.clear();                     // empty the container.
}
/*----------------------------------------------------------------------------
 *   ParamterMarshaller
 */

/**
 * constructor
 *    @param numParameters - number of parameters we're supposed to be able
 *                           to handle.
 */

ParameterMarshaller::ParameterMarshaller(std::size_t numParameters) :
    m_nParamCount(numParameters), m_pParameters(0)
{
    m_pParameters = new Double_t[numParameters];
    Double_t nan = std::nan("1");
    for (int i = 0; i < numParameters; i++) m_pParameters[i] = nan;
}
/**
 * destructor - kills off the m_pParameters array.
 */
ParameterMarshaller::~ParameterMarshaller()
{
    delete []m_pParameters;
}
/**
 * marshall
 *    Marshall the set parameters in an event into our soup:
 *
 *  @param event - reference to the event to marhsall from.
 */
void
ParameterMarshaller::marshall(CEvent& event)
{
    DopeVector& dope(event.getDopeVector());
    for (int i = 0; i < dope.size(); i++) {
        std::size_t n = dope[i];
        if (i >= m_nParamCount) {
            std::cerr << "Warning event set parameter " << n
                << " but marshaller only had " << m_nParamCount << " elements\n";
            std::cerr << "Paramter ignored\n";
        } else {
            m_pParameters[n] = event[n];           // By definition valid.
        }
    }
}
/**
 *  reset
 *     Resets all elements back to NAN... the event supplies the dope vector
 *     indicating which elments might not be NANs.
 *  @param event - evnent that modified the parameter soup.
 */
void
ParameterMarshaller::reset(CEvent& event)
{
    DopeVector& dope(event.getDopeVector());
    for (int i = 0; i < dope.size(); i++) {
        std::size_t n = dope[i];
        Double_t nan = std::nan("1");
        if (i < m_nParamCount) {            // Silent cause marshal complained.
            m_pParameters[i] = nan;
        }
    }
}
/**
 *  pointer
 *     Returns the soup pointer:
 *  @return Double_t*
 */
Double_t*
ParameterMarshaller::pointer()
{
    return m_pParameters;
}