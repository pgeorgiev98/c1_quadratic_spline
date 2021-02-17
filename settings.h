#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QColor>

struct Settings
{
	template<typename T>
	class Field
	{
	public:
		Field(Settings &settings, const char *name, T defaultValue)
			: m_settings(settings)
			, m_name(name)
			, m_defaultValue(defaultValue)
			, m_value(QVariant(settings.settings.value(name, defaultValue)).value<T>())
		{
		}

		void set(T value)
		{
			m_settings.settings.setValue(m_name, (m_value = value));
		}

		void resetToDefault()
		{
			set(m_defaultValue);
		}

		T get() const
		{
			return m_value;
		}

		const T &defaultValue() const
		{
			return m_defaultValue;
		}

	private:
		Settings &m_settings;
		const char *m_name;
		T m_defaultValue;
		T m_value;
	};

	Settings();
	static Settings *instance();

	QSettings settings;
	Field<double> controlPointSize = Field<double>(*this, "controlPointSize", 5.0);
	Field<double> snappingDistance = Field<double>(*this, "snappingDistance", 5.0);
	Field<double> lineThickness    = Field<double>(*this, "lineThickness", 2.0);
	Field<QColor> normalColor      = Field<QColor>(*this, "normalColor",      QColor::fromRgbF(0.1, 0.1, 0.1));
	Field<QColor> backgroundColor  = Field<QColor>(*this, "backgroundColor",  QColor::fromRgbF(1.0, 1.0, 1.0));
	Field<QColor> polygonColor     = Field<QColor>(*this, "polygonColor",     QColor::fromRgbF(0.5, 0.5, 0.5));
	Field<bool>   showControlPoly  = Field<bool>  (*this, "showControlPoly",  true);
};

#endif // SETTINGS_H
