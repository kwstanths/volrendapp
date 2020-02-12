#ifndef __Node_hpp__
#define __Node_hpp__

#include <qdebug.h>
#include <qgraphicsitem.h>
#include <qlist.h>
#include <qmath.h>
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qpainter.h>
#include <qdebug.h>
#include <qstyleoption.h>

#include <glwidget.h>

class Edge;
class GenericBezier;
class GraphWidget;
class QGraphicsSceneMouseEvent;

class Node : public QGraphicsObject
{
	Q_OBJECT
public:
    Node(float size_x, float size_y, float scale_x, float scale_y, GraphWidget *graphWidget, GLWidget * glwidget);

	void addBezier(GenericBezier * bezier);
	QList<GenericBezier *> beziers() const;

    void FixX(float value);

	QRectF boundingRect() const override;
	QPainterPath shape() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    GraphWidget * graph_;
    GLWidget * glwidget_;

    QList<GenericBezier *> bezierList_;
    float size_x_;
    float size_y_;
    float scale_x_;
    float scale_y_;
    bool fix_x_ = false;
    float fixed_x_ = -1;

signals:
	void posChanged(int, int);

};

#endif
