#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <QPointF>
#include <QSizeF>

class QPainter;

class BezierCurve
{
public:
	BezierCurve(QPointF p1, QPointF p2, QPointF p3);

	void draw(QPainter *painter, QSizeF scale);

private:
	QPointF p1, p2, p3;
};

#endif // BEZIERCURVE_H
