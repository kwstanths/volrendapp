#include "GenericBezier.hpp"
#include "Node.hpp"

#include <iostream>

#include "glm/glm.hpp"

GenericBezier::GenericBezier(Node * srcPoint, Node * dstPoint, float size_x, float size_y, float scale_x, float scale_y, GLWidget * glwidget, int channel, QColor color) {
	setAcceptedMouseButtons(0);

    size_x_ = size_x;
    size_y_ = size_y;
    scale_x_ = scale_x;
    scale_y_ = scale_y;
    glwidget_ = glwidget;
    channel_ = channel;
    color_ = color;

	nodes_.push_back(srcPoint);
	nodes_.push_back(dstPoint);

	points_ = new std::vector<QPointF>(2);

	srcPoint->addBezier(this);
	dstPoint->addBezier(this);

	adjust();
}

GenericBezier::GenericBezier(Node * srcPoint, Node * ctrlPoint, Node * dstPoint, float size_x, float size_y, float scale_x, float scale_y, GLWidget * glwidget, int channel, QColor color) {
	setAcceptedMouseButtons(0);

    size_x_ = size_x;
    size_y_ = size_y;
    scale_x_ = scale_x;
    scale_y_ = scale_y;
    glwidget_ = glwidget;
    channel_ = channel;
    color_ = color;

	nodes_.push_back(srcPoint);
	nodes_.push_back(ctrlPoint);
	nodes_.push_back(dstPoint);

	points_ = new std::vector<QPointF>(3);

	srcPoint->addBezier(this);
	ctrlPoint->addBezier(this);
	dstPoint->addBezier(this);

	adjust();
}

GenericBezier::GenericBezier(Node * srcPoint, Node * ctrlPoint1, Node * ctrlPoint2, Node * dstPoint, float size_x, float size_y, float scale_x, float scale_y, GLWidget * glwidget, int channel, QColor color) {
	setAcceptedMouseButtons(0);

    size_x_ = size_x;
    size_y_ = size_y;
    scale_x_ = scale_x;
    scale_y_ = scale_y;
    glwidget_ = glwidget;
    channel_ = channel;
    color_ = color;

	nodes_.push_back(srcPoint);
	nodes_.push_back(ctrlPoint1);
	nodes_.push_back(ctrlPoint2);
	nodes_.push_back(dstPoint);

	points_ = new std::vector<QPointF>(4);

	srcPoint->addBezier(this);
	ctrlPoint1->addBezier(this);
	ctrlPoint2->addBezier(this);
	dstPoint->addBezier(this);

	adjust();
}

GenericBezier::GenericBezier(std::vector<Node*>& nodes, float size_x, float size_y, float scale_x, float scale_y, GLWidget * glwidget, int channel, QColor color): nodes_(nodes) {
	setAcceptedMouseButtons(0);

	if (nodes.size() <= 1) throw std::invalid_argument("Not enough nodes");

    size_x_ = size_x;
    size_y_ = size_y;
    scale_x_ = scale_x;
    scale_y_ = scale_y;
    glwidget_ = glwidget;
    channel_ = channel;
    color_ = color;

	points_ = new std::vector<QPointF>(nodes.size());

	for (std::vector<Node *>::iterator itr = nodes_.begin(); itr != nodes_.end(); ++itr) {
		(*itr)->addBezier(this);
	}

	adjust();
}

GenericBezier::~GenericBezier() {
	delete points_;
}

Node * GenericBezier::getNode(int index) {
	return nodes_.at(index);
}

void GenericBezier::adjust() {
	for (std::vector<Node *>::iterator itr = nodes_.begin(); itr != nodes_.end(); ++itr)
		if (!(*itr)) return;

	prepareGeometryChange();
	for (unsigned int i = 0; i < nodes_.size() - 1; i++) {
		QLineF line(mapFromItem(nodes_.at(i), 0, 0), mapFromItem(nodes_.at(i + 1), 0, 0));
		qreal length = line.length();

		//if new line is big enough update to new points
		if (length > qreal(5.)) {
			points_->at(i) = line.p1();
			points_->at(i + 1) = line.p2();
		} else {
			points_->at(i) = points_->at(i + 1) = line.p1();
		}
	}

}

void GenericBezier::setScale(float scale) {
    scale_x_ = scale;
}

QRectF GenericBezier::boundingRect() const {
	for (unsigned int i = 0; i < nodes_.size(); i++) if (!nodes_.at(i)) return QRectF();
	qreal penWidth = 1;
	qreal extra = (penWidth) / 2.0;

	double x_min = find([](QPointF * a, QPointF * b) -> bool {return a->x() > b->x(); })->x();
	double y_min = find([](QPointF * a, QPointF * b) -> bool {return a->y() > b->y(); })->y();
	double x_max = find([](QPointF * a, QPointF * b) -> bool {return a->x() < b->x(); })->x();
	double y_max = find([](QPointF * a, QPointF * b) -> bool {return a->y() < b->y(); })->y();

	return QRectF(x_min, y_min, x_max - x_min, y_max - y_min).normalized().adjusted(-extra, -extra, extra, extra);
}

void GenericBezier::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
	for (std::vector<Node *>::iterator itr = nodes_.begin(); itr != nodes_.end(); ++itr)
		if (!(*itr)) return;

	painter->setPen(QPen(color_));

    // One point for every x index is the minimum number of a good looking bezier curve
	int manhattan_x = find([](QPointF * a, QPointF * b) -> bool {return a->x() < b->x(); })->x() - find([](QPointF * a, QPointF * b) -> bool {return a->x() > b->x(); })->x();
	int manhattan_y = find([](QPointF * a, QPointF * b) -> bool {return a->y() < b->y(); })->y() - find([](QPointF * a, QPointF * b) -> bool {return a->y() > b->y(); })->y();
	int min_points = sqrt(pow(manhattan_x, 2) + pow(manhattan_y, 2));

    /* Use double the sampling rate of the distance between the two points, to make sure that the transfer function will not have unset values */
    min_points = 2 * (min_points / scale_x_);
    double step = 1.0 / double(min_points);
    QPoint * points = static_cast<QPoint *>(malloc((min_points + 1) * sizeof(QPoint)));

    /* Calculate bezier intermidiate points */
	int n = nodes_.size() - 1;
#pragma omp parallel for num_threads(4) shared(points, n, min_points)
    for (int np = 0; np <= min_points; np++) {
		double x = 0;
		double y = 0;
		double t = np*step;
		for (int i = 0; i <= n; i++) {
			int coeff = binomialCoeff(n, i);
			x += coeff * pow((1 - t), n - i) * pow(t, i) * points_->at(i).x();
			y += coeff * pow((1 - t), n - i) * pow(t, i) * points_->at(i).y();
		}
        points[np].setX(x);
        points[np].setY(y);
    }

    /* Calculate actual transfer function data from the intermidiate points
     * If min_points was big enough, we should cover all values within the range
     */
    if (glwidget_ != nullptr){
        for(int i=0; i< min_points + 1; i++){
            float x = points[i].x() / scale_x_;
            float y = (size_y_ - (float) points[i].y())/scale_y_;
            /* Data are stored rgbargbargba..., and the size is 256 * 4 */
            int index = static_cast<int>(x) * 4 + channel_;
            /* The user is free to move the nodes anywhere */
            if (index < 0 || index >= 256*4) continue;
            glwidget_->transfer_function_values_[index] = glm::clamp(y, 0.0f, 1.0f);
        }
        /* Draw transfer function */
        glwidget_->SetTransferFunction();
    }

    /* Draw line */
    painter->drawPolyline(points, min_points + 1);
    delete points;
}

QPointF *  GenericBezier::find(bool(*f)(QPointF *, QPointF *)) const{

	if (points_->size() == 0) return new QPointF();
	if (points_->size() == 1) return &points_->at(0);

	QPointF * current = &points_->at(0);
	for (unsigned int i = 1; i < points_->size(); i++) {
		if (f(current, &points_->at(i))) current = &points_->at(i);
	}
	return current;
}

int GenericBezier::binomialCoeff(int n, int k) {
	int ** C = new int*[n+1];
	for (int i = 0; i < n+1; i++) C[i] = new int[k+1];

	for (int i = 0; i <= n; i++) {
		for (int j = 0; j <= std::min(i, k); j++) {
			if (j == 0 || j == i) C[i][j] = 1;
			else C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
		}
	}
	int result = C[n][k];

	for (int i = 0; i < n + 1; i++) delete C[i];
	delete C;

	return result;
}
