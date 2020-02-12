// Author: Marc Comino 2019

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QMainWindow>
#include <QCloseEvent>

#include "TFWidget.hpp"

namespace Ui {
class MainWindow;
}

namespace gui {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  virtual void show();

 private slots:
  void closeEvent (QCloseEvent *event);

  /**
   * @brief on_actionQuit_triggered Closes the application.
   */
  void on_actionQuit_triggered();

  /**
   * @brief on_actionLoad_triggered Opens a file dialog to load a PLY mesh.
   */
  void on_actionLoad_triggered();

  /**
   * @brief button_transfer_function Opens the transfer function editing tool
   */
  void button_transfer_function();

private:
  Ui::MainWindow *ui_;

    TFWidget * tf_widget_;
};

}  //  namespace gui

#endif  //  MAIN_WINDOW_H_
