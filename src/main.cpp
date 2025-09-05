#include <QApplication>

#include "controller/controller.hpp"
#include "gui/mainwindow.h"
#include "patterns/model_manager.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  s21::ModelManager& model_manager = s21::ModelManager::GetInstance();
  s21::Controller controller(model_manager);

  MainWindow window(&controller);
  window.setWindowTitle("3D Model Viewer");
  window.show();

  return app.exec();
}