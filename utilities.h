#ifndef UTILITIES_H
#define UTILITIES_H

#include <QPointF>
#include <QSizeF>

struct Utilities
{
	static QPointF scale(QPointF point, QSizeF factor)
	{
		return QPointF(point.x() * factor.width(), point.y() * factor.height());
	}

	static QPointF reverseScale(QPointF point, QSizeF factor)
	{
		return QPointF(point.x() / factor.width(), point.y() / factor.height());
	}
};

#endif // UTILITIES_H
