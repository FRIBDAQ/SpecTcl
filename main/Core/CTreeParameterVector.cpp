#include "CTreeParameterVector.h"
#include "CTreeParameter.h"

#include <sstream>

/*
   This map has a basename -> info translation.
*/
std::map<std::string, CTreeParameterVector::pTreeVectorInfo> CTreeParameterVector::m_baseNameMap;


static const double DEFAULT_LOW(0.0);
static const double DEFAULT_HIGH(100.0);

CTreeParameterVector::_TreeVectorInfo::_TreeVectorInfo() :
    s_low(DEFAULT_LOW), s_high(DEFAULT_HIGH), s_units("") {}


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
 * @param units - (optional) units of measure
 * 
 */
CTreeParameterVector::CTreeParameterVector(const char* basename, double low, double high, const char* units) : 
m_baseName(basename), m_pInfo(nullptr)
{
    m_pInfo = getInfoBlock(basename, low, high, units);
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

/**
 * operator[]
 *    Returns a reference to the tree parameter for this event picked off by index
 * @param index - which one to return.
 * @return CTreeParameter& the one seleted.
 * @throw std::out_of_range if index does not (yet?) refer to an element of the event vector.
 */
CTreeParameter&
CTreeParameterVector::operator[](size_t index) {
    CTreeParameter* p = m_pInfo->s_event.at(index);   //can throw.

    return *p;
}
/**
 * push_back
 *    adds a tree paramter to the event with the specified value. 
 * If possible, the tree parameter is gotten from the s_createdParameters vector.
 * If not  a new one is created.
 * @param value - value given tot he parameter.
 * @return CTreeParameter& referene to the tree parameter that was added to the event.
 */
CTreeParameter&
CTreeParameterVector::push_back(double value) {
    CTreeParameter* p = createEventParameter();
    *p = value;
    return *p;
}

/**
 * size - return size of the event vector.
 */
size_t
CTreeParameterVector::size() const {
    return m_pInfo->s_event.size();
}
/**
 * allocation - return the number of parameters that have been created.
 */
size_t
CTreeParameterVector::allocation() const {
    return m_pInfo->s_createdParameters.size();
}

/**
 * reset
 *    Reset the event vector to empty.  The parameters continue to exist.
 */
void
CTreeParameterVector::reset()  {
    m_pInfo->s_event.clear();    // The tree parameters get reset by SpecTcl.
}

/**
 * low - return the low limit.
 */
double
CTreeParameterVector::low() const {
     return m_pInfo->s_low;
}
/**
 * high - return the high limit
 */
double
CTreeParameterVector::high() const{
    return m_pInfo->s_high;
}
/**
 * units - return the units
 */
std::string
CTreeParameterVector::units() const {
    return m_pInfo->s_units;
}

/**
 * setLow - set the low limit
 * 
 * @param low - the new low limit 
 */
void
CTreeParameterVector::setLow(double low) {
    m_pInfo->s_low = low;    // Set for new ones
    for (auto p : m_pInfo->s_createdParameters) { // set for created ones:
        p->setStart(low);
    }
}
/**
 *  setHigh - set the high limit.
 * @param high - the high limit.
 */
void
CTreeParameterVector::setHigh(double high) {
    m_pInfo->s_high = high;
    for (auto p : m_pInfo->s_createdParameters) {
        p->setStop(high);
    }
}
/**
 * setUnits
 *   @param units - new units of measure.
 */
void
CTreeParameterVector::setUnits(const char* units) {
    m_pInfo->s_units = units;
    for (auto p : m_pInfo->s_createdParameters) {
        p->setUnit(units);
    }
}

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
 * @param units - units of measure.
 * @return pTreeVectorInfo
 */
CTreeParameterVector::pTreeVectorInfo
CTreeParameterVector::getInfoBlock(const char* name, double low, double high, const char* units) {
    pTreeVectorInfo p = getInfoBlock(name);
    p->s_low  = low;
    p->s_high = high;
    p->s_units = units;
    return p;
}

/** 
 * createEventParameter
 * 
 *    Adds a parameter to the event parameter, if possible it's gotten from the created one.
 * If not, it's created.
 * 
 * @return CTreeParameter* - pointer to the paramter, that has been added to the end of s_event
 */
CTreeParameter*
CTreeParameterVector::createEventParameter() {
    // can we use a pre-created one:

    CTreeParameter* p(nullptr);
    if (m_pInfo->s_event.size() >= m_pInfo->s_createdParameters.size()) {
        // need a new one:

        p = createParameter();
    } else {
        p = m_pInfo->s_createdParameters[m_pInfo->s_event.size()];  // use pre-existing.
    }
    m_pInfo->s_event.push_back(p);
    return p;
}

/**
 * createParameter
 *    Create a new parameter and add it to the s_createdParameters block.
 *  A pointer to the parameter is returned.
 */
CTreeParameter*
CTreeParameterVector::createParameter() {
    std::string name = nextName();
    CTreeParameter* result  = 
        new CTreeParameter(
            name, m_pInfo->s_low, m_pInfo->s_high, m_pInfo->s_units
        );
    result->Bind();   // Bind to parameter array element.
    m_pInfo->s_createdParameters.push_back(result);

    return result;
}

/**
 * nextName
 *    Compute the next parameter name.
 * @return std::string
 */
std::string
CTreeParameterVector::nextName() const {
    std::stringstream sname;
    sname << m_baseName << '(' << m_pInfo->s_createdParameters.size() << ')';

    std::string result = sname.str();

    return result;
}