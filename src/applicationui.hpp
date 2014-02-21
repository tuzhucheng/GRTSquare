#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>

#include <bb/cascades/GroupDataModel>
#include <bb/data/SqlDataAccess>
#include <bb/data/SqlConnection>

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

    // Properties used by listviews in QML
    Q_PROPERTY(bb::cascades::DataModel* routesDataModel READ routesDataModel CONSTANT)\
    Q_PROPERTY(bb::cascades::DataModel* stopsDataModel READ stopsDataModel CONSTANT)\
    Q_PROPERTY(bb::cascades::DataModel* stoptimesDataModel READ stoptimesDataModel CONSTANT)\

public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }
    Q_INVOKABLE void listRoutes();
    Q_INVOKABLE void searchStops(const QString &query);
    Q_INVOKABLE void getNextBusTimes(const QString &stop);

private slots:
    void onSystemLanguageChanged();
    void onLoadResultData(const bb::data::DataAccessReply&);
private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    // To alert the user if something has gone wrong
    void alert(const QString &message);

    // Async data access object
    bb::data::SqlConnection *sqlConnector;

    // The getter method for properties
    bb::cascades::GroupDataModel* routesDataModel() const;
    bb::cascades::GroupDataModel* stopsDataModel() const;
    bb::cascades::GroupDataModel* stoptimesDataModel() const;

    void listRoutesBuildModel(const QVariant &result);
    void searchStopsBuildModel(const QVariant &result);
    void getNextBusTimesBuildModel(const QVariant &result);

    void initRoutesDataModel();
    void initStopsDataModel();
    void initStoptimesDataModel();
    bool initDatabase();

    // The data shown by the list view.
    GroupDataModel* m_routesDataModel;
    GroupDataModel* m_stopsDataModel;
    GroupDataModel* m_stoptimesDataModel;
};

#endif /* ApplicationUI_HPP_ */
