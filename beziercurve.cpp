#include "beziercurve.h"
#include "utilities.h"

#include <QPainter>

BezierCurve::BezierCurve(QPointF p1, QPointF p2, QPointF p3)
	: p1(p1), p2(p2), p3(p3)
{
}

void BezierCurve::draw(QPainter *painter, QSizeF scale)
{
	QPen pen = painter->pen();

	QPointF p1 = Utilities::scale(this->p1, scale);
	QPointF p2 = Utilities::scale(this->p2, scale);
	QPointF p3 = Utilities::scale(this->p3, scale);

	if (s()->showControlPoly.get()) {
		pen.setStyle(Qt::DashLine);
		pen.setColor(s()->polygonColor.get());
		painter->setPen(pen);

		painter->drawLine(p1, p2);
		painter->drawLine(p2, p3);
	}

	pen.setStyle(Qt::SolidLine);
	pen.setColor(s()->normalColor.get());
	painter->setPen(pen);

	const int segmentCount = 100;
	QVector<QLineF> lines;
	lines.reserve(segmentCount);

	QPointF lastPoint = p3;
	for (int i = 1; i < segmentCount; ++i) {
		double t = double(i) / (segmentCount - 1);
		QPointF m1 = t * p1 + (1 - t) * p2;
		QPointF m2 = t * p2 + (1 - t) * p3;
		QPointF p  = t * m1 + (1 - t) * m2;
		lines.append(QLineF(lastPoint, p));
		lastPoint = p;
	}

	painter->drawLines(lines);
}
