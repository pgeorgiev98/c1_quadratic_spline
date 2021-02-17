#include "surface.h"
#include "beziercurve.h"
#include "settings.h"
#include "utilities.h"

#include <QPaintEvent>
#include <QPainter>
#include <QVector2D>

static const double epsilon = 1e-10;
static const double pointConstrainEpsilon = 1e-5;

Surface::Surface(QWidget *parent)
	: QWidget(parent)
	, m_selectedContolPoint(0)
{
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
	painter.setRenderHint(QPainter::Antialiasing, true);
	QSizeF scale = size();

	painter.setPen(QPen(s()->normalColor.get(), s()->lineThickness.get()));
	for (int i = 0; i + 2 < m_controlPoints.size(); i += 2) {
		BezierCurve c(m_controlPoints[i].position(),
					  m_controlPoints[i + 1].position(),
					  m_controlPoints[i + 2].position());
		c.draw(&painter, scale);
	}

	painter.setPen(QPen(s()->normalColor.get(), s()->lineThickness.get(), Qt::DashLine));
	for (int i = 0; i < m_barPoints.size() - 1; ++i)
		painter.drawLine(Utilities::scale(m_barPoints[i].position(), scale), Utilities::scale(m_barPoints[i + 1].position(), scale));

	painter.setPen(QPen(s()->normalColor.get(), s()->lineThickness.get()));
	for (int i = 0; i < m_controlPoints.size(); ++i) {
		QColor color = (i % 2 == 0 && i > 1 && i < m_controlPoints.size() - 2) ? s()->backgroundColor.get() : s()->normalColor.get();
		m_controlPoints[i].draw(&painter, scale, s()->normalColor.get(), color);
	}

	if (!m_barPoints.isEmpty()) {
		m_barPoints.first().draw(&painter, scale, s()->normalColor.get(), s()->normalColor.get());
		m_barPoints.last().draw(&painter, scale, s()->normalColor.get(), s()->normalColor.get());
		for (int i = 1; i < m_barPoints.size() - 1; ++i)
			m_barPoints[i].draw(&painter, scale, s()->normalColor.get(), s()->backgroundColor.get());
	}
}

void Surface::mouseMoveEvent(QMouseEvent *event)
{
	QPointF mousePos = event->pos();
	QSizeF scale = size();
	m_mousePos = Utilities::reverseScale(mousePos, scale);

	if (!m_controlPoints.isEmpty()) {
		m_controlPoints.first().move(m_mousePos);
		m_controlPoints.last().move(m_mousePos);
		for (int i = 1; i < m_controlPoints.size() - 1; ++i) {
			if (m_controlPoints[i].move(m_mousePos)) {
				if (i % 2 != 0)
					updateSpline();
				updateBar();
			}
		}
	}

	for (ControlPoint &p : m_barPoints)
		if (p.move(m_mousePos))
			updateSpline();

	update();
}

void Surface::mousePressEvent(QMouseEvent *event)
{
	QPointF mousePos = event->pos();
	QSizeF scale = size();
	m_mousePos = Utilities::reverseScale(mousePos, scale);

	bool pressed = false;

	for (int i = m_barPoints.size() - 2; i >= 1; --i) {
		if (m_barPoints[i].press(mousePos, scale)) {
			pressed = true;
			break;
		}
	}

	if (!pressed) {
		for (int i = 0; i < m_controlPoints.size(); ++i) {
			ControlPoint &p = m_controlPoints[i];
			if (p.press(mousePos, scale)) {
				selectPoint(i);
				break;
			}
		}
	}

	update();
}

void Surface::mouseReleaseEvent(QMouseEvent *event)
{
	QPointF mousePos = event->pos();
	m_mousePos = Utilities::reverseScale(mousePos, size());
	bool mustUpdate = false;
	for (ControlPoint &p : m_controlPoints)
		mustUpdate |= p.release(m_mousePos);
	for (ControlPoint &p : m_barPoints)
		mustUpdate |= p.release(m_mousePos);

	if (mustUpdate)
		updateSpline();

	updateConstraints();
}

void Surface::mouseDoubleClickEvent(QMouseEvent *event)
{
	double pointSize = s()->controlPointSize.get();
	m_mousePos = Utilities::reverseScale(event->pos(), size());

	// Create new bar point
	if (!m_controlPoints.isEmpty()) {
		if (m_barPoints.size() == 0) {
			m_barPoints.append(ControlPoint(QPointF(0.1, 0.9), pointSize));
		} else if (m_barPoints.size() == 1) {
			m_barPoints.append(ControlPoint(QPointF(0.9, 0.9), pointSize));
		} else {
			for (int i = 0; i < m_barPoints.size() - 1; ++i) {
				QPointF p = m_barPoints[i].position();
				p.setX(p.x() - m_barPoints.first().position().x());
				p.setX(p.x() * double(m_barPoints.size() - 1) / double(m_barPoints.size()));
				p.setX(p.x() + m_barPoints.first().position().x());
				m_barPoints[i].setPosition(p);
			}
			ControlPoint cp = m_barPoints.last();
			cp.setPosition((m_barPoints[m_barPoints.size() - 2].position() + cp.position()) / 2);
			m_barPoints.insert(m_barPoints.size() - 1, cp);
		}
	}

	// Update control point constraints
	if (m_controlPoints.size() >= 3) {
		QPointF a = m_controlPoints[m_controlPoints.size() - 2].position();
		QPointF b = m_controlPoints[m_controlPoints.size() - 1].position();
		m_controlPoints.insert(m_controlPoints.size() - 1, ControlPoint((a + b) / 2, pointSize));
	}

	m_controlPoints.append(ControlPoint(m_mousePos, pointSize));

	updateConstraints();
	update();
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
	updateConstraints();
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
	s()->controlPointSize.set(size);
	for (ControlPoint &p : m_controlPoints)
		p.setSize(size);
	for (ControlPoint &p : m_barPoints)
		p.setSize(size);
	update();
}

void Surface::updateConstraints()
{
	if (!m_barPoints.isEmpty()) {
		m_barPoints.first().fix();
		m_barPoints.last().fix();
		QPointF ep(pointConstrainEpsilon, 0);
		for (int i = 1; i < m_barPoints.size() - 1; ++i)
			m_barPoints[i].constrain(QLineF(m_barPoints[i - 1].position() + ep, m_barPoints[i + 1].position() - ep));
	}

	for (int i = 0; i < m_controlPoints.size(); ++i) {
		if (i % 2 == 0 && i >= 2 && i < m_controlPoints.size() - 2) {
			QPointF a = m_controlPoints[i - 1].position();
			QPointF b = m_controlPoints[i + 1].position();
			QPointF ep = (QVector2D(b - a).normalized() * pointConstrainEpsilon).toPointF();
			m_controlPoints[i].constrain(QLineF(a + ep, b - ep));
		} else {
			m_controlPoints[i].unconstrain();
		}
	}
}
