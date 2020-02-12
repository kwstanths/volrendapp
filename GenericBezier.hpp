#ifndef __GenericBezier_hpp__
#define __GenericBezier_hpp__

#include <math.h>
#include <vector>
#include <algorithm>
#include <exception>
#include <omp.h>

#include <qgraphicsitem.h>
#include <qpainter.h>

#include "glwidget.h"

class Node;

class GenericBezier : public QGraphicsItem
{
public:
	/*
		Linear Bezier with no control points
	*/
    GenericBezier(Node * srcPoint, Node * dstPoint, float size_x, float size_y, float scale_x, float scale_y, GLWidget * glwidget, int channel, QColor color = Qt::black);

	/*
		Quadratic Bezier with one control point
	*/
    GenericBezier(Node * srcPoint, Node * ctrlPoint, Node * dstPoint, float size_x, float size_y, float scale_x, float scale_y, GLWidget * glwidget, int channel, QColor color = Qt::black);

	/*
		Cubic Bezier with two control points
	*/
    GenericBezier(Node * srcPoint, Node * ctrlPoint1, Node * ctrlPoint2, Node * dstPoint, float size_x, float size_y, float scale_x, float scale_y, GLWidget * glwidget, int channel, QColor color = Qt::black);

	/*
		Bezier with multiple control points
	*/
    GenericBezier(std::vector<Node *>& nodes, float size_x, float size_y, float scale_x, float scale_y, GLWidget * glwidget, int channel, QColor color = Qt::black);

	~GenericBezier();

	Node * getNode(int index);
	void adjust();
    void setScale(float scale);

protected:
	QRectF boundingRect() const override;
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

private:
    float size_x_;
    float size_y_;
    float scale_x_;
    float scale_y_;
	QColor color_;
	std::vector<Node *> nodes_;
    std::vector<QPointF> * points_;
    GLWidget * glwidget_;
    int channel_;

	QPointF * find(bool(*f)(QPointF *, QPointF *)) const;
	int binomialCoeff(int n, int k);
};

#endif
