#include "applicationui.hpp"

#include "Route.hpp"
#include "Stop.hpp"
#include "StopTime.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/data/SqlDataAccess>
#include <bb/data/SqlConnection>
#include <bb/data/DataAccessError>
#include <bb/system/SystemDialog>

#include <QtSql/QtSql>
#include <QDebug>
#include <QDateTime>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

const QString DB_PATH = "app/native/assets/grtdata/grt.db";
const int ORIGIN_CURRENT = 0;
const int ORIGIN_FUTURE = 1;
const int SEARCH_STOPS_QUERY = 2;
const int NEXT_BUS_TIMES_CURRENT_QUERY = 3;
const int NEXT_BUS_TIMES_FUTURE_QUERY = 4;

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

    initDatabase();

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
}

void ApplicationUI::initRoutesDataModel()
{
    // Note: The Group Data Model is joining this objects tree as a child (for memory management)
	m_routesDataModel = new GroupDataModel(this);
    // GroupDataModel does not sorting by an integer key, so we don't we don't use sorting keys
    // and instead just add a "ORDER BY" clause to SQL
    m_routesDataModel->setSortingKeys(QStringList() << "routeNumber");
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
	m_stoptimesDataModel->setSortingKeys(QStringList() << "route" << "time");
	m_stoptimesDataModel->setGrouping(ItemGrouping::ByFullValue);
}

bool ApplicationUI::initDatabase()
{
    // establish connection thread to database
    sqlConnector = new SqlConnection(DB_PATH, this);

    // use signals and slots to get results when data is ready
    bool connectStatus = QObject::connect(sqlConnector, SIGNAL(reply(const bb::data::DataAccessReply&)),
            this, SLOT(onLoadResultData(const bb::data::DataAccessReply&)));

    if (connectStatus) {
    	qDebug() << "DB Connection is successful";
    	return true;
    } else {
    	qDebug() << "SQL Connector signals and slots not connected.";
    	return false;
    }
}

void ApplicationUI::onLoadResultData(const DataAccessReply& reply)
{
	if (reply.hasError()) {
		qDebug() << "Error while retrieving data with reply id " + reply.id();
	} else {
		QVariant result;
		if (reply.id() == 1) {
			result = reply.result();
			listRoutesBuildModel(result);
		} else if (reply.id() == SEARCH_STOPS_QUERY) {
			result = reply.result();
			searchStopsBuildModel(result);
		} else if (reply.id() == NEXT_BUS_TIMES_CURRENT_QUERY) {
			result = reply.result();
			getNextBusTimesBuildModel(result, ORIGIN_CURRENT);
		} else if (reply.id() == NEXT_BUS_TIMES_FUTURE_QUERY) {
			result = reply.result();
			getNextBusTimesBuildModel(result, ORIGIN_FUTURE);
		}
	}
}

void ApplicationUI::listRoutes()
{
	const QString sqlQuery = "SELECT routeName, routeNumber FROM routes "
    		" WHERE routeNumber <= ?";
			// " ORDER BY routeNumber DESC";

	QVariantList args;
	args << 202;

    sqlConnector->execute(sqlQuery, args, 1);
}

void ApplicationUI::listRoutesBuildModel(const QVariant& result)
{
	m_routesDataModel->clear();
	int recordsRead = 0;
	if( !result.isNull() ) {
		QVariantList list = result.value<QVariantList>();
		recordsRead = list.size();
		for(int i = 0; i < recordsRead; i++) {
			QVariantMap map = list.at(i).value<QVariantMap>();
			Route *route = new Route(map["routeName"].toString(), map["routeNumber"].toInt());
			Q_UNUSED(route);
			//NOTE: When adding an object to a DataModel, the DataModel sets
			//    itself as the parent of the object if no parent has already been
			//    set. Therefore, when clearing or removing an item from the data model
			//    the data model will destroy the object if it is the parent of the object.
			m_routesDataModel->insert(route);
		}
	}
	qDebug() << "Read " << recordsRead << " records succeeded";
}

void ApplicationUI::searchStops(const QString& query)
{
    const QString sqlQuery = "SELECT stopNumber, stopName FROM stops "
    		" WHERE stopNumber LIKE \'" + query + "%\' ORDER BY stopNumber DESC";
    QVariantList args = QList<QVariant>();

    sqlConnector->execute(sqlQuery, args, SEARCH_STOPS_QUERY);
}

void ApplicationUI::searchStopsBuildModel(const QVariant &result)
{
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
}

void ApplicationUI::getNextBusTimes(const QString &stop, const QString &datetime)
{
	QDateTime parsedDateTime = QDateTime::fromString(datetime, "yyyyMMdd hh:mm:ss");
	qDebug() << "Parsed date time is valid: " << parsedDateTime.isValid();
	getNextBusTimes(stop, parsedDateTime, ORIGIN_FUTURE);
}

void ApplicationUI::getNextBusTimes(const QString& stop)
{
	QDateTime datetime = QDateTime::currentDateTime();
	getNextBusTimes(stop, datetime, ORIGIN_CURRENT);
}

void ApplicationUI::getNextBusTimes(const QString &stop, const QDateTime &datetime, const int origin)
{
	QDate date = datetime.date();
	QTime time = datetime.time();

	bool nextDay = false;

	QHash<int, QString> daysOfWeekMap;
	daysOfWeekMap[1] = "monday";
	daysOfWeekMap[2] = "tuesday";
	daysOfWeekMap[3] = "wednesday";
	daysOfWeekMap[4] = "thursday";
	daysOfWeekMap[5] = "friday";
	daysOfWeekMap[6] = "saturday";
	daysOfWeekMap[7] = "sunday";

	QString timeMin, timeMax;
	timeMin = time.toString("hh:mm:ss");
	time = time.addSecs(10800);
	timeMax = time.toString("hh:mm:ss");
	if (timeMax < timeMin) {
		nextDay = true;
	}

	QString dayOfWeek = daysOfWeekMap[date.dayOfWeek()];
	QString yyyymmdd = date.toString("yyyyMMdd");

	QString sqlQuery = "SELECT serviceId FROM calendar "
					"WHERE " + dayOfWeek + " = 1 AND "
					"startDate <= \'" + yyyymmdd + "\' AND endDate >= \'" + yyyymmdd + "\'";

	// qDebug() << sqlQuery;
	QList<QString> serviceIds;

	DataAccessReply reply = sqlConnector->executeAndWait(sqlQuery);
	QVariant result;
	if (!reply.hasError()) {
		result = reply.result();
        if( !result.isNull() ) {
            QVariantList list = result.value<QVariantList>();
            int recordsRead = list.size();
            for(int i = 0; i < recordsRead; i++) {
                QVariantMap map = list.at(i).value<QVariantMap>();
                serviceIds << map["serviceId"].toString();
            }
        }
	} else {
		qDebug() << "Reply has error.";
	}

	// qDebug() << serviceIds;

	sqlQuery = "SELECT * FROM calendar_dates WHERE date = \'" + yyyymmdd + "\'";
	reply = sqlConnector->executeAndWait(sqlQuery);
	if (!reply.hasError()) {
		result = reply.result();
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
		qDebug() << "Reply has error.";
	}

	// qDebug() << serviceIds;
	QString serviceIdSQLPortion = "";
	for (int i = 0; i < serviceIds.size(); ++i) {
		if (i != 0) {
			serviceIdSQLPortion += " OR ";
		}
		serviceIdSQLPortion += "trips.serviceId = \'" + serviceIds.at(i) + "\'";
	}
	// qDebug() << serviceIdSQLPortion;

	sqlQuery = "SELECT trips.routeId, trips.tripHeadSign, stop_times.arrivalTime "
			"FROM stop_times JOIN trips ON stop_times.tripId = trips.tripId "
			"WHERE stop_times.stopId = " + stop + " AND (" + serviceIdSQLPortion + ") "
			"AND stop_times.arrivalTime >= \'" + timeMin + "\'";
	qDebug() << sqlQuery;
	QVariantList args = QList<QVariant>();

	if (origin == ORIGIN_CURRENT) {
		sqlConnector->execute(sqlQuery, args, NEXT_BUS_TIMES_CURRENT_QUERY);
	} else if (origin == ORIGIN_FUTURE) {
		sqlConnector->execute(sqlQuery, args, NEXT_BUS_TIMES_FUTURE_QUERY);
	}

}

void ApplicationUI::getNextBusTimesBuildModel(const QVariant &result, const int origin)
{
	m_stoptimesDataModel->clear();
	if( !result.isNull() ) {
		QVariantList list = result.value<QVariantList>();
		int recordsRead = list.size();
		for(int i = 0; i < recordsRead; i++) {
			QVariantMap map = list.at(i).value<QVariantMap>();
			StopTime *stopTime = new StopTime(map["routeId"].toInt(), map["arrivalTime"].toString(), map["tripHeadSign"].toString());
			Q_UNUSED(stopTime);
			m_stoptimesDataModel->insert(stopTime);
		}
		qDebug() << "Read " << recordsRead << " stoptimes records succeeded";
		if (origin == 0) {
			emit stopTimesFinishedLoadingCurrent();
		} else if (origin == 1) {
			emit stopTimesFinishedLoadingFuture();
		}
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

QDateTime ApplicationUI::getMaxDateTime()
{
	const QString sqlQuery = "SELECT MAX(endDate) FROM calendar";
	    QVariantList args = QList<QVariant>();

	DataAccessReply reply = sqlConnector->executeAndWait(sqlQuery);
	QVariant result;
	if (!reply.hasError()) {
		result = reply.result();
		if( !result.isNull() ) {
			QVariantList list = result.value<QVariantList>();
			QVariantMap map = list.at(0).value<QVariantMap>();
			return QDateTime::fromString(map["MAX(endDate)"].toString(), "yyyyMMdd");
		}
	} else {
		qDebug() << "getMaxDateTime query has error.";
	}

	QDateTime* nullDateTime = new QDateTime();
	return *nullDateTime;
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
