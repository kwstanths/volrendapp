#ifndef __GraphWidget_hpp__
#define __GraphWidget_hpp__

#include <iostream>
#include <vector>
#include <math.h>

#include <qgraphicsview.h>
#include <QKeyEvent>

#include "glwidget.h"

class Node;

class GraphWidget : public QGraphicsView
{
	Q_OBJECT

public:
    GraphWidget(float size_x, float size_y, int channel, GLWidget * glwidget, QWidget *parent = 0);
	~GraphWidget();
	std::vector<Node *>& getNodes();
    void setPoint(int node, float x, float y);
	void setBackgroundColor(QColor color);
    void DrawHistogram(std::vector<double> & histogram);

protected:
	void wheelEvent(QWheelEvent * event) override;
	bool eventFilter(QObject * object, QEvent * event) override;

private:
	QPointF mouse_position_;
	std::vector<Node *> nodes;
    GLWidget * glwidget_ = nullptr;
    std::vector<QGraphicsItem *> histogram_rects_;

    float size_x_;
    float size_y_;
    float scale_x_;
    float scale_y_;
	QGraphicsScene * scene_;

	void DrawBackground();
	void DrawLine(int index, int orientation);
    QPoint getCoords(float x, float y);
};


#endif
