#include "applicationui.hpp"

#include "Route.hpp"
#include "Stop.hpp"
#include "StopTime.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/data/SqlDataAccess>
#include <bb/data/DataAccessError>
#include <bb/system/SystemDialog>

#include <QtSql/QtSql>
#include <QDebug>
#include <QDateTime>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

const QString DB_PATH = "app/native/assets/grtdata/grt.db";

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

	// Initialize the Group Data Model before setting up our QML Scene
	// as the QML scene will bind to the data model
	initRoutesDataModel();
	initStopsDataModel();
	initStoptimesDataModel();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Expose this class to QML so that we can call its functions from C++ code
    qml->setContextProperty("app", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Get a handle to the UI controls
    //pFetchGRTDataStatus = root->findChild < TextArea* >("textarea_fetch_grt_data_status");

    // Set created root object as the application scene
    app->setScene(root);

    // Initialize the database, ensure a connection can be established
    // and that all the required tables and initial data exists
    const bool dbInited = initDatabase();

    // Inform the UI if the database was successfully initialized or not
    root->setProperty("databaseOpen", dbInited);
}

void ApplicationUI::initRoutesDataModel()
{
    // Note: The Group Data Model is joining this objects tree as a child (for memory management)
	m_routesDataModel = new GroupDataModel(this);
    // GroupDataModel does not sorting by an integer key, so we don't we don't use sorting keys
    // and instead just add a "ORDER BY" clause to SQL
    // m_dataModel->setSortingKeys(QStringList() << "routeNumber");
	m_routesDataModel->setGrouping(ItemGrouping::None);
}

void ApplicationUI::initStopsDataModel()
{
	m_stopsDataModel = new GroupDataModel(this);
	m_stopsDataModel->setGrouping(ItemGrouping::None);
}

void ApplicationUI::initStoptimesDataModel()
{
	m_stoptimesDataModel = new GroupDataModel(this);
	m_stoptimesDataModel->setGrouping(ItemGrouping::ByFullValue);
}

bool ApplicationUI::initDatabase()
{
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
	database.setDatabaseName(DB_PATH);

    // If we cannot open a connection to the database, then fail initialization
    // and display an error message
    if (database.open() == false) {
        const QSqlError error = database.lastError();
        alert(tr("Error opening connection to the database: %1").arg(error.text()));
        qDebug() << "\nDatabase NOT opened.";
        return false;
    }

    return true;
}

void ApplicationUI::listRoutes()
{
    SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);

    const QString sqlQuery = "SELECT routeName, routeNumber FROM routes "
    		" WHERE routeNumber <= 202 ORDER BY routeNumber DESC";

    QVariant result = sqlda->execute(sqlQuery);
    if (!sqlda->hasError()) {
        int recordsRead = 0;
        // The data will be displayed in a group data model
        // Clear any previous reads from the data model first
        m_routesDataModel->clear();
        if( !result.isNull() ) {
            QVariantList list = result.value<QVariantList>();
            recordsRead = list.size();
            for(int i = 0; i < recordsRead; i++) {
                QVariantMap map = list.at(i).value<QVariantMap>();
                Route *route = new Route(map["routeName"].toString(), map["routeNumber"].toString());
                Q_UNUSED(route);
                //NOTE: When adding an object to a DataModel, the DataModel sets
                //    itself as the parent of the object if no parent has already been
                //    set. Therefore, when clearing or removing an item from the data model
                //    the data model will destroy the object if it is the parent of the object.
                m_routesDataModel->insert(route);
            }
        }

        qDebug() << "Read " << recordsRead << " records succeeded";
    } else {
        alert(tr("Read records failed: %1").arg(sqlda->error().errorMessage()));
    }
}

void ApplicationUI::searchStops(const QString& query)
{
    SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);
    const QString sqlQuery = "SELECT stopNumber, stopName FROM stops "
    		" WHERE stopNumber LIKE \'" + query + "%\' ORDER BY stopNumber DESC";

    QVariant result = sqlda->execute(sqlQuery);
    if (!sqlda->hasError()) {
        int recordsRead = 0;
        m_stopsDataModel->clear();
        if( !result.isNull() ) {
            QVariantList list = result.value<QVariantList>();
            recordsRead = list.size();
            for(int i = 0; i < recordsRead; i++) {
                QVariantMap map = list.at(i).value<QVariantMap>();
                Stop *stop = new Stop(map["stopNumber"].toString(), map["stopName"].toString());
                Q_UNUSED(stop);
                m_stopsDataModel->insert(stop);
            }
        }

        qDebug() << "Read " << recordsRead << " records succeeded";
    } else {
        alert(tr("Read records failed: %1").arg(sqlda->error().errorMessage()));
    }
}

void ApplicationUI::getNextBusTimes(const QString& stop)
{
	SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);

	QDateTime datetime = QDateTime::currentDateTime();
	QDate date = datetime.date();
	QTime time = datetime.time();

	QHash<int, QString> daysOfWeekMap;
	daysOfWeekMap[1] = "monday";
	daysOfWeekMap[2] = "tuesday";
	daysOfWeekMap[3] = "wednesday";
	daysOfWeekMap[4] = "thursday";
	daysOfWeekMap[5] = "friday";
	daysOfWeekMap[6] = "saturday";
	daysOfWeekMap[7] = "sunday";

	QString dayOfWeek = daysOfWeekMap[date.dayOfWeek()];
	QString yyyymmdd = date.toString("yyyyMMdd");

	QString sqlQuery = "SELECT * FROM calendar "
					"WHERE " + dayOfWeek + " = 1 AND startDate < \'" + yyyymmdd + "\' AND endDate > \'" + yyyymmdd + "\'";

	qDebug() << sqlQuery;
	QList<QString> serviceIds;

	QVariant result = sqlda->execute(sqlQuery);
	if (!sqlda->hasError()) {
        if( !result.isNull() ) {
            QVariantList list = result.value<QVariantList>();
            int recordsRead = list.size();
            for(int i = 0; i < recordsRead; i++) {
                QVariantMap map = list.at(i).value<QVariantMap>();
                serviceIds << map["serviceId"].toString();
            }
        }
	} else {
        alert(tr("Read records failed: %1").arg(sqlda->error().errorMessage()));
    }

	qDebug() << serviceIds;

	sqlQuery = "SELECT * FROM calendar_dates WHERE date = \'" + yyyymmdd + "\'";
	result = sqlda->execute(sqlQuery);
	if (!sqlda->hasError()) {
        if( !result.isNull() ) {
            QVariantList list = result.value<QVariantList>();
            int recordsRead = list.size();
            for(int i = 0; i < recordsRead; i++) {
                QVariantMap map = list.at(i).value<QVariantMap>();
                int exceptionType = map["exception_type"].toInt();
                QString serviceId = map["serviceId"].toString();
                int index = serviceIds.indexOf(serviceId);

                if (exceptionType == 1 && index == -1) {
                	serviceIds << serviceId;
                }

                if (exceptionType == 2 && index != -1) {
                	serviceIds.removeAt(index);
                }
            }
        }
	} else {
        alert(tr("Read records failed: %1").arg(sqlda->error().errorMessage()));
    }

	qDebug() << serviceIds;
	QString serviceIdSQLPortion = "";
	for (int i = 0; i < serviceIds.size(); ++i) {
		if (i != 0) {
			serviceIdSQLPortion += " OR ";
		}
		serviceIdSQLPortion += "trips.serviceId = \'" + serviceIds.at(i) + "\'";
	}
	qDebug() << serviceIdSQLPortion;

	sqlQuery = "SELECT trips.routeId, trips.tripHeadSign, stop_times.arrivalTime "
			"FROM stop_times JOIN trips ON stop_times.tripId = trips.tripId "
			"WHERE stop_times.stopId = \'" + stop + "\' AND (" + serviceIdSQLPortion + ") "
			"ORDER BY stop_times.arrivalTime DESC;";
	qDebug() << sqlQuery;
	result = sqlda->execute(sqlQuery);
		if (!sqlda->hasError()) {
	        if( !result.isNull() ) {
	            QVariantList list = result.value<QVariantList>();
	            int recordsRead = list.size();
	            for(int i = 0; i < recordsRead; i++) {
	                QVariantMap map = list.at(i).value<QVariantMap>();
	                StopTime *stopTime = new StopTime(map["routeId"].toString(), map["arrivalTime"].toString());
	                Q_UNUSED(stopTime);
	                m_stoptimesDataModel->insert(stopTime);
	            }
	            qDebug() << "Read " << recordsRead << " stoptimes records succeeded";
	        }
		} else {
	        alert(tr("Read records failed: %1").arg(sqlda->error().errorMessage()));
	    }

}

GroupDataModel* ApplicationUI::routesDataModel() const
{
    return m_routesDataModel;
}

GroupDataModel* ApplicationUI::stopsDataModel() const
{
	return m_stopsDataModel;
}

GroupDataModel* ApplicationUI::stoptimesDataModel() const
{
	return m_stoptimesDataModel;
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("GRTSquare_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

// -----------------------------------------------------------------------------------------------
// Alert Dialog Box Functions
void ApplicationUI::alert(const QString &message)
{
    SystemDialog *dialog; // SystemDialog uses the BB10 native dialog.
    dialog = new SystemDialog(tr("OK"), 0); // New dialog with on 'Ok' button, no 'Cancel' button
    dialog->setTitle(tr("Alert")); // set a title for the message
    dialog->setBody(message); // set the message itself
    dialog->setDismissAutomatically(true); // Hides the dialog when a button is pressed.

    // Setup slot to mark the dialog for deletion in the next event loop after the dialog has been accepted.
    bool ok = connect(dialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), dialog, SLOT(deleteLater()));
    Q_ASSERT(ok);
    dialog->show();
}
