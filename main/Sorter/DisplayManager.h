#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

class DisplayManager
{
private:
    CXamine*            m_pDisplayer;          // Points to displayer object.

public:
    DisplayManager();

    CXamine* getDisplayer() const {
      return m_pDisplayer;
    }
    void setDisplayer(CXamine* am_Displayer) {
      m_pDisplayer = am_Displayer;
    }

    // Manipulate display bindings:
    UInt_t BindToDisplay (const std::string& rsName);
    void UnBindFromDisplay (UInt_t nSpec);

};

#endif // DISPLAYMANAGER_H
