#include <QtGui/QApplication>
#include "UpdateExample.h"

#include <UpdateManager.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	// Check if there are new updates available
	UpdateManager updateMgr( "LittleMozart", "Little Mozart", "pt", QDate(2011,6,30));

	// Check if the update manager shoudl be shown
	if(updateMgr.showUpdateManager())
	{
		// Update the info from the software
		updateMgr.setSoftwareData(UpdateManager::Windows, "2.1.16", 4091);
		updateMgr.setBrowserMode(false);
		updateMgr.show();
		int updateExecReturn = a.exec();

		// If the application should not run, stop now
		if (!updateMgr.canRunApp())
		{
			return updateExecReturn;
		}
	}

	// Run the application
	UpdateExample w;
	w.show();

	return a.exec();
}
