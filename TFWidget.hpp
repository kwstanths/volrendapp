#ifndef TFWIDGET_H_
#define TFWIDGET_H_

#include <QWidget>

#include <GL/glew.h>

#include "GraphWidget.hpp"
#include "glwidget.h"

class TFWidget : public QWidget {
  Q_OBJECT

 public:
  TFWidget(GLWidget * glwidget, QWidget *parent = 0);

  ~TFWidget();

  void SetHistogram(std::vector<double>& histogram);

 protected:

 private:
  GraphWidget * red_, * green_, * blue_, * alpha_;

 protected slots:

 public slots:

};

#endif  //  TFWIDGET_H_
