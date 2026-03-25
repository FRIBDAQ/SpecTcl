#include "CTreeParameterVector.h"
#include "CTreeParameter.h"

/*
   This map has a basename -> info translation.
*/
std::map<std::string, CTreeParameterVector::pTreeVectorInfo> CTreeParameterVector::m_baseNameMap;


static const double DEFAULT_LOW(0.0);
static const double DEFAULT_HIGH(100.0);

CTreeParameterVector::_TreeVectorInfo::_TreeVectorInfo() :
    s_low(DEFAULT_LOW), s_high(DEFAULT_HIGH) {}

/////////////////////////////// Utilities: //////////////////////////////////////


/**
 *  getInfoBlock
 *    This overload locates the info block associated with a base name and, if it does
 * not exist, creates it with the default low/high limits.  If the block _does_ already
 * exist, the limits are _not_ modified.
 * 
 * @param name - base name of the vector
 * @return pTreeVectorInfo
 */
CTreeParameterVector::pTreeVectorInfo
CTreeParameterVector::getInfoBlock(const char* name) {
    std::string sname = name;
    auto p = m_baseNameMap.find(sname);
    if (p == m_baseNameMap.end()) {
        m_baseNameMap[sname] = new TreeVectorInfo;
    }
    return m_baseNameMap[sname];
}
/**
 * getInfoBlock
 *    This overload locates the info block associated with the base name
 * but unconditionally sets the limits as requested:
 * 
 * @param name - base name of the parameter.
 * @param low  - low limit.
 * @param high - high limit.
 * @return pTreeVectorInfo
 */
CTreeParameterVector::pTreeVectorInfo
CTreeParameterVector::getInfoBlock(const char* name, double low, double high) {
    pTreeVectorInfo p = getInfoBlock(name);
    p->s_low  = low;
    p->s_high = high;

    return p;
}