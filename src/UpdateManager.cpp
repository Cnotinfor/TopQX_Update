#include "UpdateManager.h"
#include "ui_UpdateManager.h"
#include "Settings.h"
#include <CnotiSettings>

// Qt
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QTranslator>

/*!
	Sets the software release date. this date is used, for checking if an update is necessary or not,

	Only after \a _checkForUpdatesInterval days from the release date, the dialog for update is shown.

*/
UpdateManager::UpdateManager(const QString &applicationName, const QString &applicationTitle,
							 const QString &language, const QDate &releaseDate, QWidget *parent) :
	QWidget(parent),
	_applicationName(applicationName),
	_applicationTitle(applicationTitle),
	_language(language),
	_appAction(UpdateManager::CloseApplication),
	_ui( NULL ),
	_productId( 0 ),
	_version( "0" ),
	_platform( Windows ),
	_browserMode( true ), // enable/disable browser mode
    _releaseDate( releaseDate ),
    _checkForUpdatesInterval(14), // in days -> 14 days = 2 weeks
    _browserUrl( "http://update.site.com/" ),
	_dontShowDialogAgain( false ),
	_settings( NULL )
{
	// This is necessary, if it is a static lib
	#ifndef SHARED_LIB
		Q_INIT_RESOURCE(resources);
	#endif

	// Load the translation
	QTranslator *translation = new QTranslator( this );
	if( translation->load( ":/" + language ) )
	{
		qApp->installTranslator( translation );
		qDebug() << "[UpdateManager::UpdateManager] Translation:" << language << "loaded";
	}
	else
	{
		qWarning() << "[UpdateManager::UpdateManager] Error loading translation " << language;
	}

	_ui = new Ui::UpdateManager();
	// Setup the UI
	_ui->setupUi(this);
	_ui->stackedWidget->setCurrentIndex (WIDGET_PAGE_CHECK);
	setWindowTitle( _applicationTitle );

	// Update the %1 with the Application Title
	_ui->commandLinkButtonStart->setDescription(_ui->commandLinkButtonStart->description().arg(_applicationTitle));
	_ui->commandLinkButtonCheckUpdates->setDescription(_ui->commandLinkButtonCheckUpdates->description().arg(_applicationTitle));
	_ui->commandLinkButtonDownload->setDescription(_ui->commandLinkButtonDownload->description().arg(_applicationTitle));
	_ui->commandLinkButtonStart2->setDescription(_ui->commandLinkButtonStart2->description().arg(_applicationTitle));
	_ui->commandLinkButtonExit->setDescription(_ui->commandLinkButtonExit->description().arg(_applicationTitle));
    _ui->commandLinkButtonStart3->setDescription(_ui->commandLinkButtonStart3->description().arg(_applicationTitle));
    _ui->commandLinkButtonExit2->setDescription(_ui->commandLinkButtonExit2->description().arg(_applicationTitle));
	_ui->checkBoxDontShow->setText(_ui->checkBoxDontShow->text().arg(_applicationTitle));

	//
	connect( &_server, SIGNAL( response( const QStringList& ) ), this, SLOT( handlerServerResponse( const QStringList& ) ) );

	// Read from settings file when the last update was made
#ifdef Q_WS_MAC
	_settings = new CnotiSettings( QDir(QDir::homePath() + "/Library/Application Support/" + applicationName) , applicationName);
#else
	_settings = new CnotiSettings(applicationName, "Imagina", QSettings::UserScope );
#endif

	_lastUpdateCheck = _settings->option(Settings::LAST_UPDATE_CHECK, _releaseDate, Settings::GROUP).toDate();

	// Update auto update checkbox state
	QString autoUpdate = _settings->option(Settings::AUTO_UPDATE, Settings::YES, Settings::GROUP).toString();
	if(autoUpdate == Settings::NO)
	{
		_ui->checkBoxDontShow->setCheckState(Qt::Checked);
	}

}

UpdateManager::~UpdateManager()
{
	delete _ui;
	delete _settings;
}

/*!
  Sets the mode for checking the updates. If it uses browser mode, instead of using HTTP connection
  it opens a browser window with the information about the software.

  \sa isBrowserMode
 */
void UpdateManager::setBrowserMode(bool active)
{
	_browserMode = active;
}

/*!
  Returns if the UpdateManager should open a browser window to check for updates

  \sa setBrowsetMode
 */
bool UpdateManager::isBrowserMode() const
{
	return _browserMode;
}

/*!
  Set the Url used to check for updates using the browser.

  Usualy not necessary to change.
 */
void UpdateManager::setBrowserUrl(const QString &url)
{
	_browserUrl = url;
}

/*!
  Return the url in a QString used to open in the browser window
 */
QString UpdateManager::browserUrl() const
{
	return _browserUrl;
}

/*!
  Sets the information to be used when checking for updates. It should be called before
  calling \a show

  \sa softwarePlatform
  \sa softwareVersion
  \sa softwareId
*/
void UpdateManager::setSoftwareData(Platform platform, const QString &version, int id )
{
	_platform = platform;
	_version = version;
	_productId = id;
}

UpdateManager::Platform UpdateManager::softwarePlatform() const
{
	return _platform;
}
QString UpdateManager::softwareVersion() const
{
	return _version;
}

/*!
  Return the software Id defined previously. If it was not set before, it return 0
 */
int UpdateManager::softwareId() const
{
	return _productId;
}



/*!
  Asks if user wants to check if there are updates available.
*/
void UpdateManager::checkForUpdates(QUrl url)
{
	_server.request(url.toString());
}

/*!
  Return if the app can be executed.
  When selected to update doesn't run the app.
*/
bool UpdateManager::canRunApp() const
{
	return _appAction;
}

/*!
  Converts the Platform enumeration to a QString.
*/
QString UpdateManager::platformToString(Platform p) const
{
	switch(p)
	{
		case Windows: return "win";
		case Linux: return "linux";
		case Mac: return "mac";
		default: return "win";
	}
}

/*
	Test if the update manager should be shown or not
*/
bool UpdateManager::showUpdateManager() const
{
	// Test if the user choose not to show the dialog again
	if(_settings->option(Settings::AUTO_UPDATE, Settings::YES, Settings::GROUP).toString() == Settings::NO)
	{
		return false;
	}

	// Check if is time to show this again.
	QDate today = QDate::currentDate();
	QDate t = _lastUpdateCheck.addDays( _checkForUpdatesInterval );
	qDebug() << "[UpdateManager::showUpdateManager]" << t;
	if(today >= t)
	{
		// It already pass the number of days since the release or the last update check
		return true;
	}
	else
	{
		return false;
	}
}


/*!
  Slot executed when the user presses the check update button.
*/
void UpdateManager::on_commandLinkButtonCheckUpdates_clicked()
{
	QUrl url(_browserUrl);
	url.addQueryItem( "platform", platformToString( _platform ) );
	url.addQueryItem( "version", _version );
	url.addQueryItem( "language", _language );
	url.addQueryItem( "id", QString::number( _productId ) );
    url.addQueryItem( "browser", QString::number(_browserMode) );
	// Browser mode
	if(_browserMode)
	{
		if(QDesktopServices::openUrl(url))
		{
			qDebug() << "[UpdateManager::checkUpdateClicked] Browser opened with URL:" << url;
			// Save the date when the last update has made
			_settings->setOption(Settings::LAST_UPDATE_CHECK, QDate::currentDate(), Settings::GROUP );
		}
		else
		{
			qDebug() << "[UpdateManager::checkUpdateClicked] It was not possible to open the URL:" << url;
		}
	}
	else // if not in browser mode, it try to check of update using the application
	{
		// TODO
		_ui->progressBar->setValue( 0 );
	//	_animationProgressBar->start();
		_ui->stackedWidget->setCurrentIndex( WIDGET_PAGE_CHECKING );
		checkForUpdates(url);
	}
}

/*!
	Update the setting for auto update, when the user changes the checkbox value
*/
void UpdateManager::on_checkBoxDontShow_stateChanged(int state)
{
	if( _settings != NULL )
	{
		if(state == Qt::Checked)
		{
			_settings->setOption(Settings::AUTO_UPDATE, Settings::NO, Settings::GROUP);
		}
		else
		{
			_settings->setOption(Settings::AUTO_UPDATE, Settings::YES, Settings::GROUP);
		}
	}
}


void UpdateManager::handlerServerResponse( const QStringList& buffer )
{
	//
	// Check for errors on the server request
	//
	int bufferSize = buffer.size();

	if ( !bufferSize || bufferSize < 2 )
	{
		handleError( UpdateManager::ERROR_HTTP );
		return;
	}

//	bool ok = true;
//	UpdateManagerEnum::ServerResponses e = (UpdateManagerEnum::ServerResponses)buffer[0].toInt( &ok );

//	//
//	// Convertion it's ok
//	//
//	if ( !ok )
//	{
//		handleError( UpdateManagerEnum::ERROR_HTTP );
//		return;
//	}

	//
	// Error code from the server
	//
//	switch( e )
//	{
//	case UpdateManagerEnum::UPDATE_AVAILABLE:
//		break;
//	case UpdateManagerEnum::UPDATE_NOT_AVAILABLE:
//		_ui->commandLinkButton_update->hide ();
//		break;
//	}
	// Message
	_ui->label_downloadPage->setText(QString::fromUtf8(buffer.at (16).toStdString().c_str()));
	// Check if new version available
    QString versionCheckCode = buffer.at (19).trimmed();
	if (versionCheckCode != "0")
	{
		// Save download link
        _downloadUrl = (buffer.at (20)).trimmed();
		// Show download button
		_ui->commandLinkButtonDownload->show();
	}
	else
	{
		// Hide download button
		_ui->commandLinkButtonDownload->hide();
		// check errors
		QString errorCode = _downloadUrl = (buffer.at (20)).trimmed();
		if (!errorCode.isEmpty())
		{
			_ui->label_error->setText(errorCode);	// TODO: Get text for error code.
			_ui->stackedWidget->setCurrentIndex (WIDGET_PAGE_ERROR);
			return;
		}
	}
	_ui->stackedWidget->setCurrentIndex (WIDGET_PAGE_UPDATE);

	_settings->setOption(Settings::LAST_UPDATE_CHECK, QDate::currentDate(), Settings::GROUP );
}


void UpdateManager::handleError(UpdateManager::Error error)
{
	QString displayError;
	switch (error)
	{
	case UpdateManager::ERROR_HTTP:
		displayError = tr("Error when communicating with server.");
		break;
    case UpdateManager::NO_ERROR2:
    default:
        break;
	}

	_ui->label_error->setText(displayError);
	_ui->stackedWidget->setCurrentIndex (WIDGET_PAGE_ERROR);
}

/*!
  Slot executed when the user presses the Start button
*/
void UpdateManager::on_commandLinkButtonStart_clicked()
{
	startMainApp();
}

/*!
  Slot executed when the user presses the Start button
*/
void UpdateManager::on_commandLinkButtonStart2_clicked()
{
	startMainApp();
}

/*!
  Slot executed when the user presses the Start button
*/
void UpdateManager::on_commandLinkButtonStart3_clicked()
{
    startMainApp();
}

/*!
  Slot executed when the user presses the Start button
*/
void UpdateManager::on_commandLinkButtonDownload_clicked()
{
	startDownload();
}

/*!
  Slot executed when the user presses the Start button
*/
void UpdateManager::on_commandLinkButtonExit_clicked()
{
    exitApp();
}

/*!
  Slot executed when the user presses the Start button
*/
void UpdateManager::on_commandLinkButtonExit2_clicked()
{
    exitApp();
}

/*!
	Exit the update manager widget
*/
void UpdateManager::exitApp()
{
	hide();
	qApp->exit();
}

void UpdateManager::cancelClicked()
{
	_server.abort();
	_ui->stackedWidget->setCurrentIndex( WIDGET_PAGE_CHECK );
}

/*
  Initializes the download.
  Opens a browser.
*/
void UpdateManager::startDownload()
{
	if(QDesktopServices::openUrl(_downloadUrl))
	{
		qDebug() << "[UpdateManager::startDownload] Browser opened with URL:" << _downloadUrl;
	}
	else
	{
		qWarning() << "[UpdateManager::startDownload] It was not possible to open the URL:" << _downloadUrl;
	}
}

/*
  Exits this app with the intention to start the main app.
*/
void UpdateManager::startMainApp()
{
	qDebug() << "UpdateManager::startMainApp()";
	_appAction = UpdateManager::StartApplication;
	exitApp();
}

