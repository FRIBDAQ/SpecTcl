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


/**
 * constructor 
 *    This constructor won't modify the limits of an existing
 * info block for it.
 * 
 * @param name - the base name of the parameters this vector will create.
 */
CTreeParameterVector::CTreeParameterVector(const char* basename) : 
m_baseName(basename), m_pInfo(nullptr)
{
    m_pInfo = getInfoBlock(basename);
}
/**
 *  construtor
 *     This constructor will unconditionally set the limits of the
 * info block it gets (even if it's pre-existing).
 * 
 * @param name -base name for parameters.
 * @param low  - low limit.
 * @param high - high lmit.
 * 
 */
CTreeParameterVector::CTreeParameterVector(const char* basename, double low, double high) : 
m_baseName(basename), m_pInfo(nullptr)
{
    m_pInfo = getInfoBlock(basename, low, high);
}

/**
 * copy constructor:
 */
CTreeParameterVector::CTreeParameterVector(const CTreeParameterVector& rhs) :
    m_baseName(rhs.m_baseName), m_pInfo(rhs.m_pInfo) {}

/**
 *  assignment:
 * 
 */
CTreeParameterVector&
CTreeParameterVector::operator=(const CTreeParameterVector& rhs) {
    if (this != &rhs) {
        m_baseName = rhs.m_baseName;
        m_pInfo    = rhs.m_pInfo;
    }
    return *this;
}

/**
 * equality compare - comparing the names is sufficient
 *
 */
int 
CTreeParameterVector::operator==(const CTreeParameterVector& rhs) const {
    return m_baseName == rhs.m_baseName;
}
/**
 *  inequality compare
 */
int
CTreeParameterVector::operator!=(const CTreeParameterVector& rhs) const {
    return !operator==(rhs);
}

/**
 *  destructor
 *     for now no-op.
 * 
 */
CTreeParameterVector::~CTreeParameterVector() {}

//////////////////////////// Utilities: //////////////////////////////////////


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