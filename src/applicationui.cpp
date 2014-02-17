#include "applicationui.hpp"

#include "Route.hpp"
#include "Stop.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/data/SqlDataAccess>
#include <bb/data/DataAccessError>
#include <bb/system/SystemDialog>

#include <QtSql/QtSql>
#include <QDebug>

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
	initDataModel();
	initStopsDataModel();

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

void ApplicationUI::initDataModel()
{
    // Note: The Group Data Model is joining this objects tree as a child (for memory management)
    m_dataModel = new GroupDataModel(this);
    // GroupDataModel does not sorting by an integer key, so we don't we don't use sorting keys
    // and instead just add a "ORDER BY" clause to SQL
    // m_dataModel->setSortingKeys(QStringList() << "routeNumber");
    m_dataModel->setGrouping(ItemGrouping::None);
}

void ApplicationUI::initStopsDataModel()
{
	m_stopsDataModel = new GroupDataModel(this);
	m_stopsDataModel->setGrouping(ItemGrouping::None);
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

void ApplicationUI::readRecords()
{
    SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);

    const QString sqlQuery = "SELECT routeName, routeNumber FROM routes "
    		" WHERE routeNumber <= 202 ORDER BY routeNumber DESC";

    QVariant result = sqlda->execute(sqlQuery);
    if (!sqlda->hasError()) {
        int recordsRead = 0;
        // The data will be displayed in a group data model
        // Clear any previous reads from the data model first
        m_dataModel->clear();
        if( !result.isNull() ) {
            // If the query is successful and results are not null
            // you can access the records through the QVariantList
            // by accessing the QVariantMap object at each index for the records values.
            // ImportantNote: The alternative is to add to the model directly exposing the records values for reference via qml.
            //                You essentially get the same result as with the Person object but with less hassle.
            // m_dataModel->insertList(result.value<QVariantList>());
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
                m_dataModel->insert(route);
            }
        }

        qDebug() << "Read " << recordsRead << " records succeeded";

        if (recordsRead == 0) {
            alert(tr("The routes table is empty."));
        }
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

GroupDataModel* ApplicationUI::dataModel() const
{
    return m_dataModel;
}

GroupDataModel* ApplicationUI::stopsDataModel() const
{
	return m_stopsDataModel;
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
