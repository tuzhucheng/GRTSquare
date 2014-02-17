#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>

//#include <bb/cascades/TextArea>

#include <bb/cascades/GroupDataModel>
#include <bb/data/SqlDataAccess>

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

using namespace bb::cascades;
using namespace bb::data;

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI : public QObject
{
    Q_OBJECT

    // A property that is used by the list view in QML
    Q_PROPERTY(bb::cascades::DataModel* dataModel READ dataModel CONSTANT)\
    Q_PROPERTY(bb::cascades::DataModel* stopsDataModel READ stopsDataModel CONSTANT)\

public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }
    Q_INVOKABLE void readRecords();
    Q_INVOKABLE void searchStops(const QString &query);
private slots:
    void onSystemLanguageChanged();
private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    // To alert the user if something has gone wrong
    void alert(const QString &message);

    // The getter method for the property
    bb::cascades::GroupDataModel* dataModel() const;
    bb::cascades::GroupDataModel* stopsDataModel() const;

    // UI Control Handles
    //TextArea* pFetchGRTDataStatus;

    void initDataModel();
    void initStopsDataModel();
    bool initDatabase();

    // The data shown by the list view.
    GroupDataModel* m_dataModel;
    GroupDataModel* m_stopsDataModel;
};

#endif /* ApplicationUI_HPP_ */
