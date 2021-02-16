#include "surface.h"
#include "beziercurve.h"
#include "settings.h"

#include <QPaintEvent>
#include <QPainter>

static const double epsilon = 1e-10;

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
	const int width = 800;

	m_controlPoints.append(ControlPoint(QPointF(100, 100), 1));
	m_controlPoints.append(ControlPoint(QPointF(200, 250), 1));
	m_controlPoints.append(ControlPoint(QPointF(300, 300), 1));
	m_controlPoints.append(ControlPoint(QPointF(400, 250), 1));
	m_controlPoints.append(ControlPoint(QPointF(500, 100), 1));

	for (int i = 0; i < m_controlPoints.size(); ++i)
		if (i % 2 == 0)
			m_barPoints.append(ControlPoint(QPointF(width * 0.1 + i / 2 * width * 0.8 / (m_controlPoints.size() / 2), 500), 1));
	updateSpline();
	updateBar();

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

	for (int i = 0; i < m_barPoints.size() - 1; ++i)
		painter.drawLine(m_barPoints[i].position(), m_barPoints[i + 1].position());

	for (int i = 0; i < m_controlPoints.size(); ++i) {
		QColor color = (i % 2 == 0 ? normalColor : backgroundColor);
		m_controlPoints[i].draw(&painter, normalColor, color);
	}

	for (const ControlPoint &p : m_barPoints)
		p.draw(&painter, normalColor, backgroundColor);
}

void Surface::mouseMoveEvent(QMouseEvent *event)
{
	QPointF mousePos = event->pos();
	m_mousePos = mousePos;
	bool mustUpdateSpline = false;
	bool mustUpdateBar = false;
	for (ControlPoint &p : m_controlPoints)
		mustUpdateBar |= p.move(mousePos);
	for (ControlPoint &p : m_barPoints)
		mustUpdateSpline |= p.move(mousePos);

	if (mustUpdateSpline) {
		updateSpline();
		updateBar();
	}
	if (mustUpdateBar) {
		updateBar();
		updateSpline();
	}
	update();
}

void Surface::mousePressEvent(QMouseEvent *event)
{
	QPointF mousePos = event->pos();
	m_mousePos = mousePos;

	bool pressed = false;

	for (int i = 0; i < m_controlPoints.size(); ++i) {
		ControlPoint &p = m_controlPoints[i];
		if (p.press(mousePos)) {
			selectPoint(i);
			pressed = true;
			break;
		}
	}
	if (!pressed) {
		for (int i = 0; i < m_barPoints.size(); ++i) {
			if (m_barPoints[i].press(mousePos)) {
				pressed = true;
				break;
			}
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
	for (ControlPoint &p : m_barPoints)
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
	int barIndex = 1;
	for (int i = 0; i < m_controlPoints.size(); ++i) {
		ControlPoint &p = m_controlPoints[i];
		if (i == 0 || i == m_controlPoints.size() - 1 || i % 2 != 0) {
			p.unconstrain();
		} else {
			QPointF prev = m_controlPoints[i - 1].position();
			QPointF next = m_controlPoints[i + 1].position();

			double barPrev = m_barPoints[barIndex - 1].position().x();
			double barNext = m_barPoints[barIndex + 1].position().x();
			double barCurr = m_barPoints[barIndex].position().x();
			double k = (barCurr - barPrev) / (barNext - barPrev);

			p.setPosition(prev + (next - prev) * k);
			p.constrain(QLineF(prev, next));

			++barIndex;
		}
	}
	update();
}

void Surface::updateBar()
{
	m_barPoints.first().fix();
	m_barPoints.last().fix();
	for (int i = 1; i < m_barPoints.size() - 1; ++i) {
		QPointF prev = m_barPoints[i - 1].position();
		QPointF next = m_barPoints[i + 1].position();

		int cpIndex = i * 2;
		QPointF cpPrev = m_controlPoints[cpIndex - 1].position();
		QPointF cpNext = m_controlPoints[cpIndex + 1].position();
		QPointF cpCurr = m_controlPoints[cpIndex].position();
		QPointF cpP = cpCurr - cpPrev;
		QPointF cpD = cpNext - cpPrev;
		double k = cpP.x() < epsilon ? cpP.y() / cpD.y() : cpP.x() / cpD.x();
		m_barPoints[i].setPosition(prev + k * (next - prev));
		m_barPoints[i].constrain(QLineF(prev, next));
	}
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
	for (ControlPoint &p : m_barPoints)
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
