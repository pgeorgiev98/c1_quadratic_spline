#ifndef SURFACE_H
#define SURFACE_H

#include "controlpoint.h"

#include <QWidget>

class Surface : public QWidget
{
	Q_OBJECT
public:
	explicit Surface(QWidget *parent = nullptr);

public slots:
	void updateSpline();
	void updateBar();
	void selectPoint(int pointIndex);
	void setControlPointSize(double size);
	void deletePoint();

signals:
	void deleteEnabledChanged(bool deleteEnabled);
	void splineChanged();
	void selectionChanged();

protected:
	void paintEvent(QPaintEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;

private:
	QVector<ControlPoint> m_controlPoints;
	QVector<ControlPoint> m_barPoints;
	QPointF m_mousePos;
	int m_selectedContolPoint;
};

#endif // SURFACE_H
