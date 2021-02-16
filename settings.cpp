#include "settings.h"

static Settings *settingsInstance = nullptr;

Settings::Settings()
{
	Q_ASSERT(settingsInstance == nullptr);
	settingsInstance = this;
	QSettings::setDefaultFormat(QSettings::IniFormat);
}

Settings *Settings::instance()
{
	Q_ASSERT(settingsInstance != nullptr);
	return settingsInstance;
}
