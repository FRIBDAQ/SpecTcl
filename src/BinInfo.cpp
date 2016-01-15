#include "BinInfo.h"

namespace SpJs
{

HistContent::HistContent()
    : m_values(),
      m_underflows(2, 0),
      m_overflows(2, 0)
{}

HistContent::HistContent(const std::vector<BinInfo>& values,
                         const std::vector<double>& underflows,
                         const std::vector<double>& overflows)
    : m_values(values),
      m_underflows(underflows),
      m_overflows(overflows)
{}

std::vector<BinInfo>& HistContent::getValues() {
    return m_values;
}

void HistContent::setValues(const std::vector<BinInfo>& values)
{
    m_values = values;
}

double HistContent::getUnderflow(size_t axisIndex) const {
    return m_underflows.at(axisIndex);
}

void HistContent::setUnderflow(size_t axisIndex, double value) {
    m_underflows.at(axisIndex) = value;
}

double HistContent::getOverflow(size_t axisIndex) const {
    return m_overflows.at(axisIndex);
}

void HistContent::setOverflow(size_t axisIndex, double value) {
    m_overflows.at(axisIndex) = value;
}

}
