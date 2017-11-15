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
#include <vector>
#include <map>
#include <Rtypes.h>

class CEvent;

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
    friend class ParameterTree;
public:
    typedef std::map<std::string, TreeItemBaseClass*> Contents;
protected:
    Contents     m_contents;
    bool         m_fFree;
    
public:
    TreeFolder(const char* name);
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

/**
 * @class TreeTerminal
 *    Terminal node for the parameter tree.  In addition to the name,
 *    this item contains a parameter number.  The parameter number is the
 *    parameter id, and is used to fish the parameter from the CEvent array.
 */
class TreeTerminal : public TreeItemBaseClass
{
private:
    unsigned   m_parameterId;
public:
    TreeTerminal(const char* name, unsigned parameterId);
    
    unsigned id() const;
    virtual bool isFolder() const { return false; }
    
};
/**
 * @class ParameterTree
 *    This class is derived from the TreeFolder class, however it
 *    supplies methods that can take a set of parameters and build the
 *    tree of parameters.  In all cases the parameter tree is built with
 *    dynamic parameters.
 *
 *    Note that the top level folder is unamed (more properly the nane
 *    is an empty string).
 */
class ParameterTree : public TreeFolder
{
public:
    typedef struct _ParameterDef {
        std::string s_name;
        unsigned    s_id;
        _ParameterDef(const char* name, unsigned id) :
            s_name(name), s_id(id) {}
            
    } ParameterDef, *pParameterDef;
    
public:
    ParameterTree();
    ParameterTree(const std::vector<ParameterDef>& params);
    ~ParameterTree();
    
    void buildTree(const std::vector<ParameterDef>& params);
    void clearTree();
private:
    void addParameter(const ParameterDef& param);
    TreeFolder* makeFolderPath(const std::vector<std::string>& path);
    
    static std::vector<std::string> pathElements(const char* name);
    static void clearSubTree(TreeFolder& top);
};

/**
 * @class ParameterMarshaller
 *    Marshals parameter from the CEvent array into a pot of storage
 *    that Root could be pointed at.  Note that the marshall and reset
 *    methods only run O(n) where n is the number of _set_ parameters
 *    thanks to the dope vector hiding in the CEvent object.
 */
class ParameterMarshaller
{
private:
    std::size_t m_nParamCount;
    Double_t*   m_pParameters;
    
public:
    ParameterMarshaller(std::size_t numParameters);
    virtual ~ParameterMarshaller();
    
    void marshall(CEvent& event);
    void reset(CEvent& event);
    Double_t* pointer();
};
#endif