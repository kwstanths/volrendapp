// Author: Marc Comino 2018

#include <main_window.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QCloseEvent>

#include "./ui_main_window.h"

#include "TFWidget.hpp"

namespace gui {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);

  tf_widget_ = new TFWidget(ui_->glwidget);
}

MainWindow::~MainWindow() {
    delete tf_widget_;
    delete ui_;
}

void MainWindow::show() {
    QMainWindow::show();
}

void MainWindow::closeEvent (QCloseEvent *event) {
    tf_widget_->close();

    event->accept();
}

void MainWindow::on_actionQuit_triggered() { close(); }

void MainWindow::on_actionLoad_triggered() {
  QString filename = QFileDialog::getExistingDirectory(
      this, "Choose a directory.", ".", QFileDialog::Option::ShowDirsOnly);
  if (!filename.isNull()) {
    if (!ui_->glwidget->LoadVolume(filename)){
      QMessageBox::warning(this, tr("Error"), tr("The selected volume could not be opened."));
    }else{
        tf_widget_->SetHistogram(ui_->glwidget->GetVolumeHistogram());
    }
  }
}

void MainWindow::button_transfer_function(){    
    tf_widget_->show();
}

}  //  namespace gui
