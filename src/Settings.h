#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

namespace Settings
{
	// Group used in settings file to store information for the update
	const QString GROUP                = "Update";

	// Settings
	const QString LAST_UPDATE_CHECK    = "Last Update Check";
	const QString AUTO_UPDATE          = "Auto Update";

	// Values to be used by the settings
	const QString YES       = "1";
	const QString NO        = "0";
}

#endif // SETTINGS_H
