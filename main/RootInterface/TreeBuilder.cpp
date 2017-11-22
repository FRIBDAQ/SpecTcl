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
#include <TTree.h>
#include <TBranch.h>           // Actually probably don't need this.
#include <TDirectory.h>

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
    m_nParamCount(numParameters), m_pParameters(0), m_pMap(0)
{
    m_pParameters = new Double_t[numParameters];
    m_pMap        = new unsigned[numParameters];
    
    Double_t nan = std::nan("1");
    for (int i = 0; i < numParameters; i++) {
        m_pParameters[i] = nan;
        m_pMap[i]        = i;             // Default map is unit mapping.
    }
        
}
/**
 * destructor - kills off the m_pParameters array.
 */
ParameterMarshaller::~ParameterMarshaller()
{
    delete []m_pParameters;
    delete []m_pMap;
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
        if (n >= m_nParamCount) {
            std::cerr << "Warning event set parameter " << n
                << " but marshaller only had " << m_nParamCount << " elements\n";
            std::cerr << "Paramter ignored\n";
        } else {
            // This if is needed becaus it's possible ot invalidate a parameter
            // and that does not remove the dope vector entry for it.
            
            if (event[n].isValid()) {
                m_pParameters[m_pMap[n]] = event[n];           // By definition valid.
            }
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
        if (n < m_nParamCount) {            // Silent cause marshal complained.
            m_pParameters[m_pMap[n]] = nan;
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
/**
 * mapping
 *    Returns the mapping array:
 */
unsigned*
ParameterMarshaller::mapping()
{
    return m_pMap;
}
/*-----------------------------------------------------------------------------
 * SpecTclRootTree implementation.
 *
 */

/**
 * constructor
 *   -  Construct an appropriately sized marshaller.
 *   -  Build the tree.
 *
 *  @param name   - Name of the tree to create.
 *  @param params - vector that contains parameter definitions.  Each
 *                  parameter definnition contains a name and a parameter
 *                  id.  The id is the index of that parameter in rEvent.
 */
SpecTclRootTree::SpecTclRootTree(
    std::string name, const std::vector<ParameterTree::ParameterDef>& params
) :
  m_pMarshaller(0), m_pTree(0), m_pMap(0), m_nLastId(0), m_treeName(name)
{
    buildMarshaller(params);
    buildTree(params);
}

/**
 * Destructor
 */
SpecTclRootTree::~SpecTclRootTree()
{
  delete m_pMarshaller;
  delete m_pTree;
}
/**
 * Fill
 *    - Marshall the event.
 *    - Fill the tree
 *    - Reset the marshalled event.
 * @param event - The event to be filled into the tree.
 */
void
SpecTclRootTree::Fill(CEvent& event)
{
    m_pMarshaller->marshall(event);
    m_pTree->Fill();
    m_pMarshaller->reset(event);
}
/**
 * buildMarshaller
 *   To build the marshaller, we need to figure out the
 *   largest parameter index.  That needs a pass through the
 *   parameter definitions.
 * @param params - parameter definitions.
 */
void
SpecTclRootTree::buildMarshaller(const std::vector<ParameterTree::ParameterDef>& params)
{
    std::size_t last(0);
    for (int i = 0; i < params.size(); i++) {
        if(params[i].s_id > last) last = params[i].s_id;
    }
    
    m_pMarshaller = new ParameterMarshaller(last+1);
    
    // For good measure for now, zero out the map:
    
    m_nLastId = last;
    m_pMap    = m_pMarshaller->mapping();
    for (int i = 0; i < last; i++) {
        m_pMap[i] = 0;                       // For now in any event.
    }
}
/**
 * buildTree
 *   - Figure out the parameter hiearchy.
 *   - For each folder:
 *      .  Assign a contiguous block of slots in the marshaller.
 *      .  Create a leaf specification text for the branch.
 *      .  Add the branch to the tree.
 *  @note - until we understand how to do hierarchical trees, we'll just lay
 *          the branches out flat.  In the end, we want the branch hiearachy
 *          to accurately reflect the parameter name hiearchy decoded by the
 *          ParameterTree object
 *          
 *  @param params - parameter definitions
 */
void
SpecTclRootTree::buildTree(const std::vector<ParameterTree::ParameterDef>& params)
{
    m_pTree = new TTree(m_treeName.c_str(), m_treeName.c_str());
    unsigned freeslot = 0;                 // First free slot in the marshaller.
    ParameterTree hierarchy(params);
    
    std::string folder("SpecTcl");         // Top level folder.
    buildBranch(folder, hierarchy, freeslot);
}
/**
 * buildBranch
 *    For a folder of the parameter hierarchy:
 *    - collect the terminals (leaves).
 *    - collecte the folders (subbranches).
 *    - make the mapping for the leaves into the marshaller.
 *    - Construct the leaves description string.
 *    - Create and hook the branch into the hierarchy.
 *    - Recurse for each folder.
 *
 * @param name - name of the branch
 * @param folder - TreeFolder we need to ake a branch from
 * @param firstSlot - First available slot in m_pMap.
 * @return unsigned - Next free available slot in m_pMap.
 */
unsigned
SpecTclRootTree::buildBranch(std::string name, const TreeFolder& folder, unsigned firstSlot)
{
        const TreeFolder::Contents&      c(folder.getContents());
        std::vector<const TreeFolder*>   subfolders;
        std::vector<const TreeTerminal*> leaves;
        
        // Fill the subfolders and leaves vectors from c:
        
        for(auto p = c.begin();  p != c.end(); p++) {
            if (p->second->isFolder()) {
                subfolders.push_back(
                    reinterpret_cast<const TreeFolder*>(p->second)
                );
            } else {
                leaves.push_back(
                    reinterpret_cast<const TreeTerminal*>(p->second)
                );
            }
        }
        // Generate the branch for the leaves. - no need if there are no leaves.
        
        if (!leaves.empty()) {
            std::pair<unsigned, void*> mapInfo = mapParameters(firstSlot, leaves);
            std::string              leafspecs = createLeafSpecs(leaves);
            m_pTree->Branch(name.c_str(), mapInfo.second, leafspecs.c_str());
            firstSlot = mapInfo.first;             // Next unused slot.
        }
        
        // For each subfolder make a branch whose name is the name.subfolder
        
        for (int i = 0; i < subfolders.size(); i++) {
            std::string subName = subfolders[i]->getName();
            firstSlot = buildBranch(subName, *(subfolders[i]), firstSlot);
        }
        
        return firstSlot;
}
/**
 * mapParameters
 *    Creates a mapping beetween the parameter ids in the leaves passed in
 *    and a contiguous set of slots in the marshall array.
 *
 *  @param firstSlot - first free slot in the marshalling array.
 *  @param leaves    - Vector of pointers to leaf descriptions.
 *  @return std::pair<unsigned, void*> - first is the next availabel marshall
 *          slot number.  second is a pointer to the start of the parameters.
 *  @note This should not be called if there are no leaves.
 */
std::pair<unsigned, void*>
SpecTclRootTree::mapParameters(
    unsigned firstSlot, std::vector<const TreeTerminal*>& leaves
)
{
    std::pair<unsigned, void*> result;
    
    result.first  =  firstSlot + leaves.size();
    result.second = &(m_pMarshaller->pointer()[firstSlot]);
    
    for (int i = 0; i < leaves.size(); i++) {
        m_pMap[leaves[i]->id()] = firstSlot++;  // Assign a mapping.
    }
    
    return result;
}
/**
 * createLeafSpecs
 *    Creates a root leaf specification string of the form
 *    name1/D:name2/D:...nameLast/D
 * @param leaves - vector of pointer to leaf descriptions.
 * @return the leaf specification.
 */
std::string
SpecTclRootTree::createLeafSpecs(std::vector<const TreeTerminal*>& leaves)
{
    std::string result;
    
    for (int i =0; i < leaves.size(); i++) {
        result += leaves[i]->getName();
        result += "/D:";
    }
    
    result.pop_back();           // remove trailing ':'
    return result;
}
