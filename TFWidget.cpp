#include <TFWidget.hpp>

#include "QVBoxLayout"
#include "qlabel.h"

#include "GraphWidget.hpp"

TFWidget::TFWidget(GLWidget * glwidget, QWidget *parent) : QWidget(parent) {

    setFocusPolicy(Qt::StrongFocus);

    /* Create four graph widgets, each one for each channel, 0-r, 1-g, 2-b, a-3 */
    red_ = new GraphWidget(250, 1, 0, glwidget, this);
    green_ = new GraphWidget(250, 1, 1, glwidget, this);
    blue_ = new GraphWidget(250, 1, 2, glwidget, this);
    alpha_ = new GraphWidget(250, 1, 3, glwidget, this);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addWidget(new QLabel("red"));
    layout->addWidget(red_);
    layout->addWidget(new QLabel("green"));
    layout->addWidget(green_);
    layout->addWidget(new QLabel("blue"));
    layout->addWidget(blue_);
    layout->addWidget(new QLabel("alpha"));
    layout->addWidget(alpha_);

    setLayout(layout);
}

void TFWidget::SetHistogram(std::vector<double>& histogram){
    red_->DrawHistogram(histogram);
    green_->DrawHistogram(histogram);
    blue_->DrawHistogram(histogram);
    alpha_->DrawHistogram(histogram);
}

TFWidget::~TFWidget() {

}

