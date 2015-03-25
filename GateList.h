#ifndef GATELIST_H
#define GATELIST_H

#include <QString>
#include <vector>
#include <memory>
class TCutG;

class GateList
{
public:
    using iterator = std::vector<std::unique_ptr<TCutG> >::iterator;

public:
    GateList();

    void addGate(std::unique_ptr<TCutG> cut);
    iterator getGate(const QString& name);

    iterator begin() { return m_gates.begin(); }
    iterator end() { return m_gates.end(); }

private:
    std::vector<std::unique_ptr<TCutG> > m_gates;
};

#endif // GATELIST_H
