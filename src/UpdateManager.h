#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include "webservice.h"
#include "UpdateManagerGlobal.h"

// Qt
#include <QWidget>
#include <QDate>

class CnotiSettings;

namespace Ui {
	class UpdateManager;
}

class UPDATEMANAGER_EXPORT UpdateManager : public QWidget
{
	Q_OBJECT
	Q_ENUMS( Platform )

private:

	enum Error
	{
		NO_ERROR2 = 0,
		ERROR_HTTP
	};

	enum ServerResponses
	{
		/*!No errors during registration of the serial number.*/
		UPADATE_NO_ERROR = 0,
		UPDATE_AVAILABLE,
		UPDATE_NOT_AVAILABLE
	};


public:

	enum AppOptions
	{
		/*! Start button isn't pressed before close widget.*/
		CloseApplication = 0,
		/*! Start button is pressed before close widget.*/
		StartApplication
	};

	enum Platform
	{
		Mac,
		Windows,
		Linux
	};

	explicit UpdateManager( const QString &applicationName, const QString &applicationTitle,
							const QString &language, const QDate &releaseDate, QWidget *parent = 0);
	~UpdateManager();

	void setBrowserMode(bool active);
	bool isBrowserMode() const;
	void setBrowserUrl(const QString &url);
	QString browserUrl() const;
	void setSoftwareData(Platform platform, const QString &version, int id );
	Platform softwarePlatform() const;
	QString softwareVersion() const;
	int softwareId() const;

	bool showUpdateManager() const;

	void checkForUpdates(QUrl url);
	bool canRunApp() const;

private:
	QString platformToString(Platform p) const;

private:
	Ui::UpdateManager *_ui;
	WebService         _server;
	AppOptions         _appAction;
	bool               _browserMode;
	QString            _browserUrl;
	QString            _downloadUrl;
	int                _checkForUpdatesInterval; // days
	Platform           _platform;
	QString            _version;
	int                _productId;
	QString            _language;
	QString            _applicationTitle; // Already translated
	QString            _applicationName; // to be used for CnotiSettings
	CnotiSettings     *_settings;
	QDate              _lastUpdateCheck;
	QDate              _releaseDate;
	bool               _dontShowDialogAgain;


	enum Pages{
		WIDGET_PAGE_CHECK,
		WIDGET_PAGE_CHECKING,
		WIDGET_PAGE_UPDATE,
		WIDGET_PAGE_ERROR
	};

	// Methods
	void exitApp();
	void startDownload();
	void startMainApp();

private slots:
	void handlerServerResponse(const QStringList &buffer);
	void handleError(Error error);
	void cancelClicked();
	void on_checkBoxDontShow_stateChanged(int state);
	void on_commandLinkButtonStart_clicked();
	void on_commandLinkButtonStart2_clicked();
    void on_commandLinkButtonStart3_clicked();
	void on_commandLinkButtonCheckUpdates_clicked();
	void on_commandLinkButtonDownload_clicked();
	void on_commandLinkButtonExit_clicked();
    void on_commandLinkButtonExit2_clicked();
};

#endif // UPDATEMANAGER_H
