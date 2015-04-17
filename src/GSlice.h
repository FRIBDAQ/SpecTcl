#ifndef GSLICE_H
#define GSLICE_H

#include <QObject>
#include <QString>
#include <QMetaType>

class TLine;
class TFrame;
class QRootCanvas;

namespace SpJs 
{
  class Slice;
}

/*! Graphical Slice
 *
 * This bundles together two lines and the SpecTcl information
 * for a slice. 
 *
 * \todo Insert a SpJs::GateInfo into this.
 */
class GSlice : public QObject
{
    Q_OBJECT
public:
    explicit GSlice(const SpJs::Slice& info);

    /*! Constructor
     */
    explicit GSlice(QObject *parent = 0,
                    const QString& name = QString(),
                    const QString& param = QString(),
                    double xLow = 0,
                    double xHigh = 0,
                    QRootCanvas* pCanvas=nullptr);

    virtual ~GSlice();

    /*! Assignment
     *
     * QObjects are not copyable because they have an identity. However
     * their state can be copied.
     */
    GSlice& operator=(const GSlice& rhs);
    
    // Get values of low and high 
    double getXLow() const;
    double getXHigh() const;

    // Get the graphical lines
    TLine* getXLowLine() { return m_pLow; }
    TLine* getXHighLine() { return m_pHigh; }

    QString getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    QString getParameter() const { return m_parameter; }
    void setParameter(const QString& param) { m_parameter = param; }

public slots:
    //! Draw 
    void draw(QRootCanvas* canvas);
    void draw();

    //! Update the lines
    void setXLow(double x);
    void setXHigh(double x);

    //! Slot for resizing
    void frameChanged();
    void frameChanged(TFrame* frame);

    void nameChanged(const QString& name);

private:
    QString m_name;
    TLine* m_pLow;
    TLine* m_pHigh;
    QRootCanvas* m_pCanvas;
    QString m_parameter;
};


#endif // GSLICE_H
