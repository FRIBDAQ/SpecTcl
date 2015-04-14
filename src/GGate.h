#ifndef GGATE_H
#define GGATE_H

#include <QObject>

#include <GateInfo.h>

#include <memory>
#include <vector>
#include <utility>

#include <TROOT.h>
#include <TCutG.h>
#include <iostream>

/**! Make a TCutG un-special in the eyes of ROOT
 *
 *  ROOT tries to own TCutG objects and will automatically
 *  delete them from under our feet. This simple derived 
 *  class makes sure to remove the TCutG from the list of
 *  specials immediately after they have been created.
 *
 */ 
class MyCutG : public TCutG 
{
  public:

  //! \brief Construct from name and number of point
  MyCutG(const char* name, int n) 
   : TCutG(name, n) 
  {
    auto pSpecials = gROOT->GetListOfSpecials();
    pSpecials->Remove(pSpecials->FindObject(name));
  }

  //! \brief Construct from name and number of points and data
  MyCutG(const char* name, int n, double *x, double *y) 
   : TCutG(name, n, x, y) 
  {
    auto pSpecials = gROOT->GetListOfSpecials();
    pSpecials->Remove(pSpecials->FindObject(name));
  }

  //! \brief Destructor
  virtual ~MyCutG()
  {
  }
};

/*! A Graphical Gate
 *
 *  This is a class that bundles together a graphical entity (i.e. MyCutG) and 
 *  the gate information from SpecTcl. It wraps these things in to look like 
 *  any other QObject.
 *
 */
class GGate : public QObject
{
    Q_OBJECT
public:
    /*! \brief Constructor
     *
     * Creates a new SpecTcl gate information.
     *
     * \param name    name of gate (\todo make this parameter go away)
     * \param info    spectcl gate information
     * \param parent  parent object that would own this.
     */
    explicit GGate(const QString& name,
                   const SpJs::GateInfo2D& info,
                   QObject* parent = nullptr);
    
    /*! \brief Assignment operator
     *
     * Performs a deep copy of the object. The fact that this is a QObject 
     * means that we cannot have a copy constructor but we can certainly 
     * create the same effect by creating a unique object whose state will
     * be copied.
     *
     * \param rhs   object whose state will be copied
     *
     * \returns reference to this.
     */
    GGate& operator=(const GGate& rhs);

    // Get/Set name
    QString getName() const;
    void setName(const QString& name);


    /*! \brief Get the name of the parameter for x-axis */
    QString getParameterX() const;


    /*! \brief Get the name of the parameter for y-axis */
    QString getParameterY() const;




    /*! \brief Retrieve list of point defining cut 
     *
     * \returns list of paired points. {{x0,y0}, {x1,y1}, {x2,y2}, ...}
     */
    std::vector<std::pair<double, double> > getPoints() const;



    /*! \brief Sets the state according to new gate information
     *
     * This is basically the same thing as assignment. 
     *
     * \param info  gate informaton from SpecTcl
     */
    void setInfo(const SpJs::GateInfo2D& info);



    /*! \brief Appends a point to graphical entity and gate info */
    void appendPoint(double x, double y);



    /*! \brief Removes last point from graphical entity and gate info */
    void popBackPoint();



    TCutG* getGraphicObject() { return m_pCut.get(); }

    /*! \brief Draw on the current TPad */
    void draw();



    /*! \brief Retrieves the type of gate.
     *
     *  \retval either SpJs::BandGate or SpJs::ContourGate
     */
    SpJs::GateType getType() const;




    /*! \brief Changes the type of the gate
     *
     *  THis actually changes the type of the gate information.
     *
     *  \param type   either SpJs::BandGate or SpJs::ContourGate
     */
    void setType(SpJs::GateType type);

public slots:
   /*! */
    void onPointChanged(int index, double x, double y);

    /*! \brief Updates the name */
    void onNameChanged(const QString& name);



    /// Member data 
private:
    QString m_name;
    std::unique_ptr<TCutG> m_pCut;
    std::unique_ptr<SpJs::GateInfo2D> m_info;

};

#endif // GGATE_H
