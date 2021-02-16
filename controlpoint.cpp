#include "controlpoint.h"

#include <QColor>
#include <QPainter>

static QPointF constrainedPosition(QPointF pos, QLineF line)
{
	if (line.p1() == line.p2())
		return line.p1();

	QPointF p1 = line.p1();
	QPointF p2 = line.p2();
	QPointF dp = p2 - p1;
	QPointF rdp(-dp.y(), dp.x());
	double d = rdp.x() / rdp.y();
	double l = (pos.x() - p1.x() + (p1.y() - pos.y()) * d) / (dp.x() - dp.y() * d);
	return p1 + dp * qBound(0.0, l, 1.0);
}

void ControlPoint::constrain(QLineF line)
{
	m_constrainLine = line;
	m_p = constrainedPosition(m_p, line);
}

void ControlPoint::fix()
{
	m_constrainLine = QLineF(m_p, m_p);
}

void ControlPoint::unconstrain()
{
	m_constrainLine.reset();
}

bool ControlPoint::isHoveredBy(QPointF point) const
{
	double dx = m_p.x() - point.x();
	double dy = m_p.y() - point.y();
	return dx * dx + dy * dy <= m_size * m_size;
}

bool ControlPoint::press(QPointF pos)
{
	m_isDragged = isHoveredBy(pos);
	if (m_isDragged)
		m_offset = m_p - pos;

	return m_isDragged;
}

bool ControlPoint::release(QPointF pos)
{
	if (m_isDragged) {
		move(pos);
		m_isDragged = false;
	}

	return m_isDragged;
}

bool ControlPoint::move(QPointF pos)
{
	if (m_isDragged) {
		if (m_constrainLine)
			m_p = constrainedPosition(pos + m_offset, *m_constrainLine);
		else
			m_p = pos + m_offset;
	}

	return m_isDragged;
}

void ControlPoint::draw(QPainter *painter, QColor borderColor, QColor fillColor) const
{
	painter->setPen(borderColor);
	painter->setBrush(fillColor);
	painter->drawEllipse(m_p, m_size, m_size);
}
