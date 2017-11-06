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

/** @file:  ParametersToTree
 *  @brief: Given the parameter dictionary figure out the root tree structure.
 */

/**
 * SpecTcl parameter names imply a hierarchical structure where the period
 * is the path separator, thus, for example, the parameter a.b.c
 * Implies a top level folder a subfolder b and a parameter c in that subfolder:
 *
 * \verbatim
 *    a                (folder)
 *    +--> b           (folder)
 *         +----> c    (parameter).
 *         
 * \endverbatim
 *  This file defines classes that are able to convert this implied hierarchy
 *  into a real hierarchy.
 *
 *   The following objects will be defined:
 *      *  TreeItemBaseClass - This provides several common interfaces between
 *                             the object types in the tree that will be
 *                             constructed.
 *      *  TreeFolder        - A folder that will only contain other items.
 *      *  TreeItem          - A parameter definition.
 *      *  TreeItemVector    - A vector of parameters with a base name
 *      *                      (to be implemented later).
 *      *  TreeBuilder       - Class that can build parameter trees.
 * 
 * @note - We are not going to support folders with the same name as parameters
 *         While SpecTcl supports them, using them is insanity of the worst sort.
 */

#ifndef TREEBUILDER_H
#define TREEBUILDER_H

#include <string>
#include <map>

/**
 * @class TreeItemBaseClass
 *     The base class for all objects that  can live in the tree.
 */
class TreeItemBaseClass {
private:
    std::string  m_name;
public:
    TreeItemBaseClass(const char* name);
    virtual ~TreeItemBaseClass() {}            // We are not a final class.
    
    // Selector(s):
    
    std::string getName() const;
    
    // Interface that must be supported by base classes:
    
    virtual bool isFolder() const = 0;     // True if item is a container.
};

/**
 * @class TreeFolder
 *     Objects from this class represent folders that can contain other
 *     TreeItemBaseClass derived objects (folders, items, or vectors).
 *
 *     Note that there's an assumption the children are dynamically allocated.
 *     unless otherwise set:
 */
class TreeFolder : public TreeItemBaseClass
{
public:
    typedef std::map<std::string, TreeItemBaseClass*> Contents;
private:
    Contents     m_contents;
    bool         m_fFree;
    TreeFolder(const char* name);
public:
    virtual ~TreeFolder();
    
    // Operations:
    
    void addItem(TreeItemBaseClass* pItem);
    const Contents& getContents()    const;
    Contents::const_iterator begin() const;
    Contents::const_iterator end()   const;
    size_t size()                    const;
    
    void freeStorage(bool yesno);
    
    // Base class interface implement:
    
    virtual bool isFolder() const {return true;}
};
#endif