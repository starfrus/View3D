/**
 * @file mainwindow.h
 * @brief Заголовочный файл для главного окна приложения 3DViewer.
 *
 * Содержит объявление класса MainWindow - основного окна приложения с GUI
 * элементами для загрузки, отображения и трансформации 3D-моделей.
 *
 * @authors lioncoco, starfrus, melonyna
 * @version 1.0
 * @date 2025
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColorDialog>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QTextStream>

#include "../controller/controller.hpp"
#include "glwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Главное окно приложения 3DViewer.
 *
 * Реализует графический интерфейс пользователя с элементами управления
 * для загрузки 3D-моделей, их отображения и применения трансформаций.
 * Использует Qt для создания GUI и взаимодействия с пользователем.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  /**
   * @brief Конструктор главного окна.
   *
   * @param controller Указатель на контроллер приложения.
   * @param parent Родительский виджет (по умолчанию nullptr).
   */
  explicit MainWindow(s21::Controller* controller, QWidget* parent = nullptr);

  /**
   * @brief Деструктор главного окна.
   *
   * Освобождает память, занимаемую UI элементами.
   */
  ~MainWindow();

 private slots:
  /**
   * @brief Обработчик нажатия кнопки загрузки модели.
   *
   * Открывает диалог выбора файла и загружает выбранную модель.
   */
  void onLoadButtonClicked();

  /**
   * @brief Обработчик изменения значения слайдера перемещения по X.
   *
   * @param value Новое значение слайдера.
   */
  void onTranslateXSliderChanged(int value);

  /**
   * @brief Обработчик изменения значения слайдера перемещения по Y.
   *
   * @param value Новое значение слайдера.
   */
  void onTranslateYSliderChanged(int value);

  /**
   * @brief Обработчик изменения значения слайдера перемещения по Z.
   *
   * @param value Новое значение слайдера.
   */
  void onTranslateZSliderChanged(int value);

  /**
   * @brief Обработчик изменения значения слайдера поворота вокруг X.
   *
   * @param value Новое значение слайдера.
   */
  void onRotateXSliderChanged(int value);

  /**
   * @brief Обработчик изменения значения слайдера поворота вокруг Y.
   *
   * @param value Новое значение слайдера.
   */
  void onRotateYSliderChanged(int value);

  /**
   * @brief Обработчик изменения значения слайдера поворота вокруг Z.
   *
   * @param value Новое значение слайдера.
   */
  void onRotateZSliderChanged(int value);

  /**
   * @brief Обработчик изменения значения слайдера масштабирования.
   *
   * @param value Новое значение слайдера.
   */
  void onScaleSliderChanged(int value);

  /**
   * @brief Обработчик изменения текста в поле ввода перемещения по X.
   */
  void onTranslateXEditChanged();

  /**
   * @brief Обработчик изменения текста в поле ввода перемещения по Y.
   */
  void onTranslateYEditChanged();

  /**
   * @brief Обработчик изменения текста в поле ввода перемещения по Z.
   */
  void onTranslateZEditChanged();

  /**
   * @brief Обработчик изменения текста в поле ввода поворота вокруг X.
   */
  void onRotateXEditChanged();

  /**
   * @brief Обработчик изменения текста в поле ввода поворота вокруг Y.
   */
  void onRotateYEditChanged();

  /**
   * @brief Обработчик изменения текста в поле ввода поворота вокруг Z.
   */
  void onRotateZEditChanged();

  /**
   * @brief Обработчик изменения текста в поле ввода масштаба.
   */
  void onScaleFactorEditChanged();
  /**
   * @brief Обработчик изменения цвета рёбер.
   *
   * Изменняет цвет рёбер.
   */
  void onEdgeColorButtonClicked();
  /**
   * @brief Обработчик изменения цвета вершин.
   *
   * Изменяет цвет вершин
   */
  void onVertexColorButtonClicked();
  /**
   * @brief Обработчик изменения цвета фона.
   *
   * Изменяет цвет фона
   */
  void onBackgroundColorButtonClicked();

  void setButtonColor(QPushButton* button, const QColor& color) {
    button->setStyleSheet(QString("background-color: %1; border: 1px solid "
                                  "black; border-radius: 4px;")
                              .arg(color.name()));
  }

 private:
  Ui::MainWindow* ui;  ///< Указатель на сгенерированный UI

  GLWidget* glWidget;  ///< Виджет для отрисовки 3D модели

  double res_sdvigX_{0.0};  ///< Текущее значение перемещения по X
  double res_sdvigY_{0.0};  ///< Текущее значение перемещения по Y
  double res_sdvigZ_{0.0};  ///< Текущее значение перемещения по Z

  double res_virtX_{0.0};  ///< Текущее значение поворота вокруг X
  double res_virtY_{0.0};  ///< Текущее значение поворота вокруг Y
  double res_virtZ_{0.0};  ///< Текущее значение поворота вокруг Z

  double res_scale_ = 1.0;  ///< Текущее значение масштаба

  s21::Controller* controller_;  ///< Указатель на контроллер приложения

  /**
   * @brief Настраивает соединения сигналов и слотов.
   *
   * Подключает все UI элементы к соответствующим обработчикам.
   */
  void setupConnections();

  /**
   * @brief Обновляет информационную панель.
   *
   * Устанавливает значения по умолчанию для элементов информации.
   */
  void updateInfoPanel();

  /**
   * @brief Обновляет информационную панель данными из модели.
   *
   * Заполняет панель информацией о количестве вершин и рёбер модели.
   */
  void updateInfoPanelFromModel();
};

#endif  // MAINWINDOW_H