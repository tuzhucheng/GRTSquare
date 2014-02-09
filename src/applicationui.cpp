/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"

#include "Route.hpp"

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

const QString DB_PATH = "./data/grtSquareDatabase.db";

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

	// Initialize the Group Data Model before setitng up our QML Scene
	// as the QML scene will bind to the data model
	initDataModel();

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

    //qDebug() << dbInited;

    // Inform the UI if the database was successfully initialized or not
    root->setProperty("databaseOpen", dbInited);
}

void ApplicationUI::initDataModel()
{
    // Note: The Group Data Model is joining this objects tree as a child (for memory management)
    m_dataModel = new GroupDataModel(this);
    m_dataModel->setSortingKeys(QStringList() << "routeID");
    m_dataModel->setGrouping(ItemGrouping::None);
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

    SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);
    sqlda->execute("DROP TABLE IF EXISTS routes");
    if(!sqlda->hasError()) {
    	qDebug() << "Table dropped.";
    } else {
    	const DataAccessError error = sqlda->error();
    	alert(tr("Drop table error: %1").arg(error.errorMessage()));//.arg(error.text()));
    }

    const QString createSQL = "CREATE TABLE routes "
                              "  (routeID INTEGER PRIMARY KEY AUTOINCREMENT, "
    						  "  routeLongName VARCHAR, "
                              "  routeNumber INTEGER);";
    sqlda->execute(createSQL);
    if(!sqlda->hasError()) {
        qDebug() << "Table created.";
    } else {
        const DataAccessError error = sqlda->error();
        alert(tr("Create table error: %1").arg(error.errorMessage()));//.arg(error.text()));
        return false;
    }

    buildRoutes(sqlda);

    return true;
}

void ApplicationUI::buildRoutes(SqlDataAccess *sqlda)
{
	QFile data("app/native/assets/grtdata/gtfs/routes.txt");
	if (data.open(QIODevice::ReadOnly)) {
		QTextStream in(&data);
		QString line, routeLongName, routeNumber;
		QStringList list;
		int atLine = 0;
		while (!in.atEnd()) {
			line = in.readLine(0);
			if (atLine != 0) {
				list = line.split(",");
				routeLongName = list[0];
				routeNumber = list[1];
				qDebug() << routeLongName;
				QVariantList route;
				route << routeLongName << routeNumber;
				sqlda->execute("INSERT INTO routes"
						"    (routeLongName, routeNumber) "
						"    VALUES (:routeLongName, :routeNumber)", route);
				if(!sqlda->hasError()) {
					// qDebug() << "success";
				} else {
					// If 'exec' fails, error information can be accessed via the error function
					// the last error is reset every time exec is called.
					const DataAccessError error = sqlda->error();
					alert(tr("Create record error: %1").arg(error.errorMessage()));
				}
			}
			atLine++;
		}
	} else {
		qDebug() << "File open operation failed.";
	}
	data.close();
}

void ApplicationUI::readRecords()
{
    SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);

    const QString sqlQuery = "SELECT routeID, routeLongName, routeNumber FROM routes";

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
                Route *route = new Route(map["routeID"].toString(), map["routeLongName"].toString(), map["routeNumber"].toString());
                qDebug() << map["routeLongName"].toString();
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

GroupDataModel* ApplicationUI::dataModel() const
{
    return m_dataModel;
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
