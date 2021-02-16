#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include <QPointF>
#include <QColor>

class QPainter;

class ControlPoint
{
public:
	ControlPoint() : ControlPoint(QPointF(0, 0), 1) {}
	ControlPoint(QPointF p, double size) : m_p(p), m_size(size) {}

	bool isHoveredBy(QPointF point) const;
	bool press(QPointF pos);
	bool release(QPointF pos);
	bool move(QPointF pos);
	void draw(QPainter *painter, QColor borderColor, QColor fillColor) const;

	QPointF position() const
	{
		return m_p;
	}

	void setPosition(QPointF position)
	{
		m_p = position;
	}

	bool isDragged() const
	{
		return m_isDragged;
	}

	void setSize(double size)
	{
		m_size = size;
	}

private:
	QPointF m_p;
	QPointF m_offset;
	bool m_isDragged;
	double m_size;
};

#endif // CONTROLPOINT_H
