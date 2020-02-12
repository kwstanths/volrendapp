#include "GraphWidget.hpp"
#include "GenericBezier.hpp"
#include "Node.hpp"

#include "QBrush"

GraphWidget::GraphWidget(float size_x, float size_y, int channel, GLWidget * glwidget, QWidget *parent) : QGraphicsView(parent), size_x_(size_x), size_y_(size_y)
{
	viewport()->installEventFilter(this);

    /* Scale axis to fit the graphs */
    scale_x_ = 2.5;
    scale_y_ = 150;
	size_x_ = size_x_ * scale_x_;
	size_y_ = size_y_ * scale_y_;
    glwidget_ = glwidget;

    /* Create the scene */
	scene_ = new QGraphicsScene(this);
    scene_->setSceneRect(-20, -25, 720, 200);
	setRenderHint(QPainter::Antialiasing);
    DrawBackground();

    /* Push five nodes in the scene */
    nodes.push_back(new Node(size_x_, size_y_, scale_x_, scale_y_, this, glwidget));
    nodes.push_back(new Node(size_x_, size_y_, scale_x_, scale_y_, this, glwidget));
    nodes.push_back(new Node(size_x_, size_y_, scale_x_, scale_y_, this, glwidget));
    nodes.push_back(new Node(size_x_, size_y_, scale_x_, scale_y_, this, glwidget));
    nodes.push_back(new Node(size_x_, size_y_, scale_x_, scale_y_, this, glwidget));
    nodes.push_back(new Node(size_x_, size_y_, scale_x_, scale_y_, this, glwidget));

    /* Initialize the positions of the nodes */
    /* Fix first the x coordiante of the first and last node so that the function covers all the range */
    /*
     * If you remove these two FixX() lines, then it will work just fine,
     * and you will have more flexibility, but the transfer function will
     * "remember" the old values. Everytime you move a node, the lines
     * connected to that node will set new values to the transfer function,
     * for that range. If a range is not covered, then the values for that
     * range are the old ones set.
     */
    nodes[0]->setPos(getCoords(-1, -0.02));
    nodes[0]->FixX(-1);
    nodes[1]->setPos(getCoords(50, -0.02));
    nodes[2]->setPos(getCoords(100, -0.02));
    nodes[3]->setPos(getCoords(150, -0.02));
    nodes[4]->setPos(getCoords(200, -0.02));
    nodes[5]->setPos(getCoords(256, -0.02));
    nodes[5]->FixX(256);

    scene_->addItem(nodes[0]);
    scene_->addItem(nodes[1]);
    scene_->addItem(nodes[2]);
    scene_->addItem(nodes[3]);
    scene_->addItem(nodes[4]);
    scene_->addItem(nodes[5]);

    /* Create bezier curves */
    /* Here I just create lines, since it's two nodes only, but if more
     * nodes are used, then it will be a bezier function
    */
    /* Pass glwidget as an argument if you wish to use this function to set transfer function values,
     * otherwise, pass nullptr */
    scene_->addItem(new GenericBezier(nodes[0], nodes[1], size_x_, size_y_, scale_x_, scale_y_, glwidget, channel, Qt::black));
    scene_->addItem(new GenericBezier(nodes[1], nodes[2], size_x_, size_y_, scale_x_, scale_y_, glwidget, channel, Qt::black));
    scene_->addItem(new GenericBezier(nodes[2], nodes[3], size_x_, size_y_, scale_x_, scale_y_, glwidget, channel, Qt::black));
    scene_->addItem(new GenericBezier(nodes[3], nodes[4], size_x_, size_y_, scale_x_, scale_y_, glwidget, channel, Qt::black));
    scene_->addItem(new GenericBezier(nodes[4], nodes[5], size_x_, size_y_, scale_x_, scale_y_, glwidget, channel, Qt::black));
    /* If you wish to have a beizer curve, you can remove the last two lines, and use the following line instead */
//    scene_->addItem(new GenericBezier(nodes[3], nodes[4], nodes[5], size_x_, size_y_, scale_x_, scale_y_, glwidget, channel, Qt::black));

	setScene(scene_);
    setFixedSize(740, 205);
}

GraphWidget::~GraphWidget() {
	//for (unsigned int i = 0; i < nodes.size(); i++) delete nodes[i];
	//delete scene_;
}

std::vector<Node *>& GraphWidget::getNodes(){
	return nodes;
}

void GraphWidget::setPoint(int node, float x, float y){
	if (node >= nodes.size()) return;

	nodes[node]->setPos(getCoords(x/scale_x_, y));
	nodes[node]->update();
}

void GraphWidget::setBackgroundColor(QColor color) {
	QBrush brush(color);
	scene_->setBackgroundBrush(brush);
}

void GraphWidget::DrawHistogram(std::vector<double> & histogram) {

    /* Delete old histogtam, if it exists */
    for(size_t i=0; i < histogram_rects_.size(); i++){
        scene_->removeItem(histogram_rects_[i]);
        delete histogram_rects_[i];
    }

    /* Draw new one */
    histogram_rects_.resize(histogram.size());
    for(size_t i=0; i<histogram.size(); i++){
        QGraphicsItem * rect = scene_->addRect(i * scale_x_, size_y_ - histogram[i] * scale_y_, scale_x_, histogram[i] * scale_y_, Qt::DotLine, QBrush(QColor(180, 180 , 180), Qt::SolidPattern));
        rect->setZValue(-1);
        histogram_rects_[i] = rect;
    }

}

void GraphWidget::wheelEvent(QWheelEvent * event) {
    /* zoom */
	qreal scaleFactor = pow((double)2, event->delta() / 240.0);

	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100) return;

	scale(scaleFactor, scaleFactor);

	centerOn(mouse_position_);
	QPointF delta_viewport_pos = mouse_position_ - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
	QPointF viewport_center = mapFromScene(mouse_position_) - delta_viewport_pos;
	centerOn(mapToScene(viewport_center.toPoint()));

}

bool GraphWidget::eventFilter(QObject * object, QEvent * event) {
    /* Record mouse position */
	if (event->type() == QEvent::MouseMove) {
		QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
		mouse_position_ = mapToScene(mouse_event->pos());
	}

	Q_UNUSED(object)
	return false;
}

void GraphWidget::DrawBackground() {
	setBackgroundColor(QColor("#fff7e6"));

    /* Draw main axes lines */
	scene_->addLine(0, -5000, 0, 5000, Qt::SolidLine);
	scene_->addLine(-5000 , size_y_ , 5000, size_y_ , Qt::SolidLine);

    /* Draw secondary axes lines */
    for (int i = 0; i <= size_x_; i += 50) DrawLine(i, 0);

    for (float i = 0; i <= size_y_; i += 0.2) DrawLine(i, 1);

    /* Draw "1" label */
	QGraphicsTextItem * y_label = new QGraphicsTextItem;
    y_label->setPos(-20, -15);
    y_label->setPlainText("1");
	scene_->addItem(y_label);
}

void GraphWidget::DrawLine(int index, int orientation) {
	if (orientation == 0) {
		//vertical
		QGraphicsTextItem * line = new QGraphicsTextItem;
		line->setPos(index * scale_x_ - 14, size_y_ + 5);
		line->setPlainText(QString::number(index));
		scene_->addItem(line);
		scene_->addLine(index * scale_x_, 5000, index * scale_x_, -5000, Qt::DotLine);
	} else if (orientation == 1) {
		//horizontal
		QGraphicsTextItem * line = new QGraphicsTextItem;
		line->setPos(-25, size_y_ -index*scale_y_);
		line->setPlainText(QString::number(index));
		scene_->addItem(line);
		scene_->addLine(-5000, size_y_ - index * scale_y_, 5000, size_y_ - index * scale_y_, Qt::DotLine);
	}
}

QPoint GraphWidget::getCoords(float x, float y) {
	return QPoint(x*scale_x_, size_y_ - y*scale_y_);
}
