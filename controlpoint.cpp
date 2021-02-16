#include "controlpoint.h"

#include <QColor>
#include <QPainter>

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
