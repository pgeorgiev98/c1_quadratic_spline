#include "surface.h"
#include "beziercurve.h"
#include "settings.h"

#include <QPaintEvent>
#include <QPainter>

static const QColor backgroundColor = QColor::fromRgbF(1, 1, 1);
static const QColor normalColor = QColor::fromRgbF(0, 0, 0);
static const QColor hoveredColor = QColor::fromRgbF(0.8, 0, 0);
static const QColor dragColor = QColor::fromRgbF(0, 0, 0.8);
static const QColor selectedColor = QColor::fromRgbF(0, 0.8, 0);
static const double lineThickness = 1.0;

Surface::Surface(QWidget *parent)
	: QWidget(parent)
	, m_selectedContolPoint(0)
{
	m_controlPoints.append(ControlPoint(QPointF(100, 100), 1));
	m_controlPoints.append(ControlPoint(QPointF(200, 250), 1));
	m_controlPoints.append(ControlPoint(QPointF(300, 300), 1));
	m_controlPoints.append(ControlPoint(QPointF(400, 250), 1));
	m_controlPoints.append(ControlPoint(QPointF(500, 100), 1));
	updateSpline();

	QPalette pal = palette();
	pal.setColor(QPalette::Window, QColor::fromRgbF(1, 1, 1));
	setPalette(pal);

	setAutoFillBackground(true);
	setMouseTracking(true);
	setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

void Surface::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setPen(QPen(normalColor, lineThickness));
	for (int i = 0; i + 2 < m_controlPoints.size(); i += 2) {
		BezierCurve c(m_controlPoints[i].position(),
					  m_controlPoints[i + 1].position(),
					  m_controlPoints[i + 2].position());
		c.draw(&painter);
	}

	for (int i = 0; i < m_controlPoints.size(); ++i) {
		QColor color = (i % 2 == 0 ? normalColor : backgroundColor);
		m_controlPoints[i].draw(&painter, normalColor, color);
	}
}

void Surface::mouseMoveEvent(QMouseEvent *event)
{
	QPointF mousePos = event->pos();
	m_mousePos = mousePos;
	bool mustUpdate = false;
	for (ControlPoint &p : m_controlPoints)
		mustUpdate |= p.move(mousePos);

	if (mustUpdate)
		updateSpline();
	update();
}

void Surface::mousePressEvent(QMouseEvent *event)
{
	QPointF mousePos = event->pos();
	m_mousePos = mousePos;

	for (int i = 0; i < m_controlPoints.size(); ++i) {
		ControlPoint &p = m_controlPoints[i];
		if (p.press(mousePos)) {
			selectPoint(i);
			break;
		}
	}

	update();
}

void Surface::mouseReleaseEvent(QMouseEvent *event)
{
	QPointF mousePos = event->pos();
	m_mousePos = mousePos;
	bool mustUpdate = false;
	for (ControlPoint &p : m_controlPoints)
		mustUpdate |= p.release(mousePos);

	if (mustUpdate)
		updateSpline();
}

void Surface::mouseDoubleClickEvent(QMouseEvent *)
{
}

void Surface::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete) {
		deletePoint();
		event->accept();
	}
}


void Surface::updateSpline()
{
	for (int i = 0; i < m_controlPoints.size(); ++i) {
		ControlPoint &p = m_controlPoints[i];
		if (i == 0 || i == m_controlPoints.size() - 1 || i % 2 != 0) {
			p.unconstrain();
		} else {
			QPointF prev = m_controlPoints[i - 1].position();
			QPointF next = m_controlPoints[i + 1].position();
			p.constrain(QLineF(prev, next));
		}
	}
	update();
}

void Surface::selectPoint(int pointIndex)
{
	if (pointIndex < 0 || pointIndex >= m_controlPoints.size())
		return;
	m_selectedContolPoint = pointIndex;

	emit selectionChanged();
	update();
}

void Surface::setControlPointSize(double size)
{
	Settings::instance()->controlPointSize.set(size);
	for (ControlPoint &p : m_controlPoints)
		p.setSize(size);
	update();
}

void Surface::deletePoint()
{
	/*
	m_controlPoints.removeAt(m_selectedContolPoint);
	selectPoint(qMax(0, m_selectedContolPoint - 1));
	updateSpline();
	emit deleteEnabledChanged(m_controlPoints.size() > 2);
	*/
}
