/**
 * @file glwidget.h
 * @brief Заголовочный файл для OpenGL виджета отрисовки 3D-моделей.
 *
 * Содержит объявление класса GLWidget - виджета для отображения 3D-моделей
 * с использованием OpenGL. Поддерживает вращение, масштабирование и настройку
 * визуальных параметров отображения.
 *
 * @authors lioncoco, starfrus, melonyna
 * @version 1.0
 * @date 2025
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GL/glu.h>

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QSettings>
#include <QString>
#include <QTimer>
#include <QWheelEvent>
#include <QtMath>
#include <vector>

#include "../model/model.hpp"

struct Colors {
  float r, g, b;
};
/**
 * @class GLWidget
 * @brief OpenGL виджет для отрисовки 3D-моделей.
 *
 * Наследуется от QOpenGLWidget и QOpenGLFunctions для реализации
 * отрисовки 3D-моделей в каркасном режиме. Поддерживает интерактивное
 * управление с помощью мыши: вращение, масштабирование.
 */
class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

 public:
  /**
   * @brief Конструктор виджета OpenGL.
   *
   * @param parent Родительский виджет (обычно MainWindow).
   */
  explicit GLWidget(QWidget* parent = nullptr);

  /**
   * @brief Дуструктор виджета OpenGL.
   */
  ~GLWidget() { saveConfig(); }

  /**
   * @brief Устанавливает данные модели для отрисовки.
   *
   * @param vertices Указатель на вектор вершин модели.
   * @param edges Вектор рёбер модели (пары индексов вершин).
   */
  void setModelData(const std::vector<s21::Vertex>* vertices,
                    const std::vector<std::pair<size_t, size_t>> edges);

  // --- Настройки отображения ---

  /**
   * @brief Получить цвет линий модели.
   */
  Colors getLineColor();
  /**
   * @brief Устанавливает цвет линий модели.
   *
   * @param r Красная компонента цвета (0.0 - 1.0).
   * @param g Зелёная компонента цвета (0.0 - 1.0).
   * @param b Синяя компонента цвета (0.0 - 1.0).
   */
  void setLineColor(float r, float g, float b);

  /**
   * @brief Получить цвет вершин модели.
   */
  Colors getVertexColor();
  /**
   * @brief Устанавливает цвет вершин модели.
   *
   * @param r Красная компонента цвета (0.0 - 1.0).
   * @param g Зелёная компонента цвета (0.0 - 1.0).
   * @param b Синяя компонента цвета (0.0 - 1.0).
   */
  void setVertexColor(float r, float g, float b);

  /**
   * @brief Получить толщину линий модели.
   */
  float getLineWidth();
  /**
   * @brief Устанавливает толщину линий модели.
   *
   * @param width Толщина линий в пикселях.
   */
  void setLineWidth(float width);

  /**
   * @brief Получить цвет фона модели.
   */
  Colors getBackgroundColor();
  /**
   * @brief Устанавливает цвет фона виджета.
   *
   * @param r Красная компонента цвета (0.0 - 1.0).
   * @param g Зелёная компонента цвета (0.0 - 1.0).
   * @param b Синяя компонента цвета (0.0 - 1.0).
   */
  void setBackgroundColor(float r, float g, float b);

  /**
   * @brief Получить тип линии(сплошная/пунктирная).
   */
  bool getDottedFacets();
  /**
   * @brief Установить тип линии(сплошная/пунктирная).
   *
   * @param val true если пунктирная.
   */
  void setDottedFacets(bool val);

  /**
   * @brief Получить признак отображения вершин.
   */
  bool getDisplayVertex();
  /**
   * @brief Установить признак отображения вершин.
   *
   * @param val true если отображать.
   */
  void setDisplayVertex(bool val);

  /**
   * @brief Получить размер вершин модели.
   */
  float getVertexSize();
  /**
   * @brief Установить размер вершин модели.
   *
   * @param val размер вершины.
   */
  void setVertexSize(float val);

  /**
   * @brief Получить тип вершин модели.
   */
  bool getRoundVertex();
  /**
   * @brief Установить тип вершин модели.
   *
   * @param val true круглая, false квадрат.
   */
  void setRoundVertex(bool val);

  /**
   * @brief Получить тип проекции.
   */
  bool getCentralProjection();
  /**
   * @brief Установить тип проекции.
   *
   * @param val true - центральная, false - параллельная
   */
  void setCentralProjection(bool val);

 protected:
  /**
   * @brief Инициализация OpenGL контекста.
   *
   * Вызывается один раз при создании виджета для настройки
   * OpenGL функций и параметров отрисовки.
   */
  void initializeGL() override;

  /**
   * @brief Обработка изменения размера виджета.
   *
   * @param w Новая ширина виджета.
   * @param h Новая высота виджета.
   */
  void resizeGL(int w, int h) override;

  /**
   * @brief Отрисовка сцены.
   *
   * Основной метод отрисовки, вызывается при каждом обновлении экрана.
   */
  void paintGL() override;

 private slots:
  /**
   * @brief Обработчик таймера.
   *
   * Вызывается регулярно для обновления отображения (60 FPS).
   */
  void onTimer();

 private:
  float defaultColor_ = 0.1;
  float defaultColor2_ = 0.9;
  float defaultSize_ = 5.0f;
  /**
   * @brief Загрузить данные из конфига.
   */
  void loadConfig();
  /**
   * @brief Сохранить данные в конфиг.
   */
  void saveConfig();
  /**
   * @brief Отрисовка линий.
   */
  void drawLines();
  /**
   * @brief Отрисовка вершин.
   */
  void drawVertex();
  /**
   * @brief Установка проекции.
   */
  void setupProjection();
  const std::vector<s21::Vertex>* vertices_ =
      nullptr;  ///< Указатель на вершины модели
  std::vector<std::pair<size_t, size_t>> edges_;  ///< Рёбра модели

  // --- Параметры вращения ---
  float angle_x_ = 0.0f;   ///< Угол вращения вокруг оси X
  float angle_y_ = 0.0f;   ///< Угол вращения вокруг оси Y
  QPoint last_mouse_pos_;  ///< Последняя позиция мыши

  // --- Параметры масштабирования ---
  float scale_ = 1.0f;  ///< Коэффициент масштабирования

  // --- Настройки визуализации ---
  QSettings* settings_;  ///< Объект для работы с ini-файлом
  QString config_path_ = "conf.ini";  ///< Путь к конфигурационному файлу

  QTimer* timer_;  ///< Таймер для регулярного обновления отображения
};

#endif  // GLWIDGET_H