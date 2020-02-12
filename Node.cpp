#include "Node.hpp"
#include "GraphWidget.hpp"
#include "GenericBezier.hpp"

Node::Node(float size_x, float size_y, float scale_x, float scale_y, GraphWidget *graphWidget, GLWidget * glwidget) : graph_(graphWidget), glwidget_(glwidget), size_x_(size_x), size_y_(size_y), scale_x_(scale_x), scale_y_(scale_y) {
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
}

void Node::addBezier(GenericBezier * bezier) {
    /* Each node holds the bezier curbes he is connected with */
    bezierList_ << bezier;
	bezier->adjust();
}

QList<GenericBezier*> Node::beziers() const {
	return bezierList_;
}

void Node::FixX(float value){
    fix_x_ = true;
    fixed_x_ = value;
}

QRectF Node::boundingRect() const {
	qreal adjust = 0;
    return QRectF(-5 - adjust, -15 - adjust, 64 + adjust, 64 + adjust);
}

QPainterPath Node::shape() const {
	QPainterPath path;
    path.addEllipse(-7, -7, 14, 14);
	return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    /* Paint the node, the circle and the coordinates */
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::darkGray);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-7, -7, 14, 14);

	int x_temp = pos().x() / scale_x_;

	QString x = QString::number(x_temp);
	QString y = QString::number((size_y_ - qFloor(pos().y()))/scale_y_);

    painter->drawText(-6, -6, x + ", " + y);
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
    /* If node changed, recalculate all the bezier curves */
	switch (change) {
	case ItemPositionHasChanged:
		foreach(GenericBezier *bezier, bezierList_)
			bezier->adjust();
		break;
	default:
		break;
	};

	return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	update();
	QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	update();

	QString y = QString::number((size_y_ - qFloor(pos().y())) / scale_y_);
	int x_position = pos().x();
	emit posChanged(x_position, y.toInt());

	QGraphicsItem::mouseReleaseEvent(event);

    /* At mouse releave event, make sure you updateGL(), because glwidget_->SetTransferFunction() may not always update */
    glwidget_->updateGL();
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

    update();
    QGraphicsItem::mouseMoveEvent(event);

    /* If x is fixed, restrict it */
    if (fix_x_){
        if (event->scenePos().x() > (fixed_x_ * scale_x_)|| event->scenePos().x() < (fixed_x_ * scale_x_)){
            this->setX(fixed_x_ * scale_x_);
        }
    }


}
