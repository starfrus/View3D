#include "glwidget.h"

/**
 * @brief Конструктор виджета OpenGL
 * @param parent Родительский виджет (обычно MainWindow)
 *
 * Инициализирует таймер для регулярного обновления (анимации),
 * подключает слот onTimer к сигналу таймера и включает отслеживание мыши.
 */
GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent), timer_(new QTimer(this)) {
  // Подключаем таймер: каждый раз, когда он срабатывает — вызываем onTimer
  connect(timer_, &QTimer::timeout, this, &GLWidget::onTimer);
  timer_->start(16);  // Запускаем таймер с интервалом ~16 мс (~60 FPS)
  settings_ = new QSettings(config_path_, QSettings::IniFormat);
  loadConfig();
}

/**
 * @brief Устанавливает данные модели для отрисовки
 * @param vertices Вершины модели
 * @param edges Рёбра модели (пары индексов вершин)
 *
 * Копирует данные модели и нормализует её положение и масштаб,
 * чтобы она хорошо помещалась в поле зрения. После этого обновляет отображение.
 */
void GLWidget::setModelData(
    const std::vector<s21::Vertex>* vertices,
    const std::vector<std::pair<size_t, size_t>> edges) {
  vertices_ = vertices;
  edges_ = edges;
  update();  // Перерисовать
}

/**
 * @brief Инициализация OpenGL-контекста
 *
 * Вызывается один раз при создании виджета.
 * Устанавливает цвет фона и включает тест глубины для правильного
 * 3D-отображения.
 */
void GLWidget::initializeGL() {
  this->initializeOpenGLFunctions();  // Инициализация функций OpenGL

  // Устанавливаем цвет очистки буфера (фон)
  glClearColor(settings_->value("background_red_").toFloat(),
               settings_->value("background_green_").toFloat(),
               settings_->value("background_blue_").toFloat(), 1.0f);

  glEnable(GL_DEPTH_TEST);  // Включаем буфер глубины
  glDepthFunc(GL_LESS);  // Глубина: чем меньше значение, тем ближе
}

/**
 * @brief Обработка изменения размера окна
 * @param w Новая ширина
 * @param h Новая высота
 *
 * Устанавливает область отображения (viewport) и матрицу проекции
 * (перспектива).
 */
void GLWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);  // Устанавливаем viewport на всю область виджета

  glMatrixMode(GL_PROJECTION);  // Переключаемся на матрицу проекции
  glLoadIdentity();     // Сбрасываем матрицу
  gluPerspective(45.0,  // Угол обзора
                 w / static_cast<double>(h),  // Соотношение сторон
                 0.1,  // Ближняя плоскость отсечения
                 100.0);  // Дальняя плоскость отсечения
}

/**
 * @brief Отрисовка сцены
 *
 * Очищает экран, устанавливает камеру, применяет трансформации и рисует рёбра
 * модели.
 */
void GLWidget::paintGL() {
  glClearColor(settings_->value("background_red_").toFloat(),
               settings_->value("background_green_").toFloat(),
               settings_->value("background_blue_").toFloat(), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Камера
  glTranslatef(0.0f, 0.0f, -3.5f);
  glScalef(scale_, scale_, scale_);

  // Вращение
  glRotatef(angle_x_, 1.0f, 0.0f, 0.0f);
  glRotatef(angle_y_, 0.0f, 1.0f, 0.0f);

  setupProjection();
  drawLines();
  drawVertex();
}
/**
 * @brief Отрисовка линий.
 */
void GLWidget::drawLines() {
  if (settings_->value("dotted_facets_").toBool()) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x00FF);
  } else {
    glDisable(GL_LINE_STIPPLE);
  }
  // Стиль линий
  glLineWidth(settings_->value("facets_size_").toFloat());
  glColor3f(settings_->value("facets_red_").toFloat(),
            settings_->value("facets_green_").toFloat(),
            settings_->value("facets_blue_").toFloat());

  // Рисуем, только если данные есть
  if (vertices_ && !edges_.empty() && !vertices_->empty()) {
    glBegin(GL_LINES);
    for (const auto& edge : edges_) {
      // Проверяем, что индексы в пределах
      if (edge.first >= vertices_->size() || edge.second >= vertices_->size()) {
        continue;
      }
      const auto& v1 = (*vertices_)[edge.first];
      const auto& v2 = (*vertices_)[edge.second];
      glVertex3f(v1.x, v1.y, v1.z);
      glVertex3f(v2.x, v2.y, v2.z);
    }
    glEnd();
  }
}
/**
 * @brief Отрисовка вершин.
 */
void GLWidget::drawVertex() {
  if (settings_->value("display_vertexes_").toBool() &&
      settings_->value("vertexes_size_").toFloat() > 0) {
    glPointSize(settings_->value("vertexes_size_").toFloat());
    glColor3f(settings_->value("vertexes_red_").toFloat(),
              settings_->value("vertexes_green_").toFloat(),
              settings_->value("vertexes_blue_").toFloat());

    if (settings_->value("round_vertexes_").toBool()) {
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_POINT_SMOOTH);
      glDisable(GL_BLEND);
    }
    if (vertices_ && !vertices_->empty()) {
      glColor3f(settings_->value("vertexes_red_").toFloat(),
                settings_->value("vertexes_green_").toFloat(),
                settings_->value("vertexes_blue_").toFloat());
      glBegin(GL_POINTS);
      for (const auto& vertex : *vertices_) {
        glVertex3f(vertex.x, vertex.y, vertex.z);
      }
      glEnd();
    }
  }
}
/**
 * @brief Получить признак отображения вершин.
 */
bool GLWidget::getDisplayVertex() {
  return settings_->value("display_vertexes_").toBool();
}
/**
 * @brief Установить признак отображения вершин.
 *
 * @param val true если отображать.
 */
void GLWidget::setDisplayVertex(bool val) {
  settings_->setValue("display_vertexes_", val);
}
/**
 * @brief Получить тип проекции.
 */
bool GLWidget::getCentralProjection() {
  return settings_->value("central_projection_").toBool();
}
/**
 * @brief Установить тип проекции.
 *
 * @param val true - центральная, false - параллельная
 */
void GLWidget::setCentralProjection(bool val) {
  settings_->setValue("central_projection_", val);
}
/**
 * @brief Получить тип линии(сплошная/пунктирная).
 */
bool GLWidget::getDottedFacets() {
  return settings_->value("dotted_facets_").toBool();
}
/**
 * @brief Установить тип линии(сплошная/пунктирная).
 *
 * @param val true если пунктирная.
 */
void GLWidget::setDottedFacets(bool val) {
  settings_->setValue("dotted_facets_", val);
}
/**
 * @brief Получить тип вершин модели.
 *
 * @param val true круглая, false квадрат.
 */
bool GLWidget::getRoundVertex() {
  return settings_->value("round_vertexes_").toBool();
}
/**
 * @brief Установить тип вершин модели.
 *
 * @param val true круглая, false квадрат.
 */
void GLWidget::setRoundVertex(bool val) {
  settings_->setValue("round_vertexes_", val);
}
/**
 * @brief Установка проекции.
 */
void GLWidget::setupProjection() {
  glTranslated(0, 0, -3.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (!settings_->value("central_projection_").toBool()) {
    glOrtho(-1, 1, -1, 1, -1, 10);
  } else {
    glFrustum(-1, 1, -1, 1, 0.5, 10);
  }

  glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Обработчик таймера
 *
 * Вызывается каждые ~16 мс. Пока просто запрашивает перерисовку.
 * Можно использовать для анимации (например, автовращения).
 */
void GLWidget::onTimer() {
  saveConfig();
  update();  // Перерисовка сцены
}

// --- Методы для настройки внешнего вида ---
/**
 * @brief Получить цвет линий модели.
 */
Colors GLWidget::getLineColor() {
  Colors col;
  col.r = settings_->value("facets_red_").toFloat();
  col.g = settings_->value("facets_green_").toFloat();
  col.b = settings_->value("facets_blue_").toFloat();
  return col;
}
/**
 * @brief Устанавливает цвет линий
 * @param r Красный (0.0 — 1.0)
 * @param g Зелёный (0.0 — 1.0)
 * @param b Синий (0.0 — 1.0)
 */
void GLWidget::setLineColor(float r, float g, float b) {
  settings_->setValue("facets_red_", r);
  settings_->setValue("facets_green_", g);
  settings_->setValue("facets_blue_", b);
  update();  // Применяем изменения
}
/**
 * @brief Получить цвет вершин модели.
 */
Colors GLWidget::getVertexColor() {
  Colors col;
  col.r = settings_->value("vertexes_red_").toFloat();
  col.g = settings_->value("vertexes_green_").toFloat();
  col.b = settings_->value("vertexes_blue_").toFloat();
  return col;
}
/**
 * @brief Устанавливает цвет вершин
 * @param r Красный (0.0 — 1.0)
 * @param g Зелёный (0.0 — 1.0)
 * @param b Синий (0.0 — 1.0)
 */
void GLWidget::setVertexColor(float r, float g, float b) {
  settings_->setValue("vertexes_red_", r);
  settings_->setValue("vertexes_green_", g);
  settings_->setValue("vertexes_blue_", b);
  update();  // Применяем изменения
}
/**
 * @brief Получить толщину линий модели.
 */
float GLWidget::getLineWidth() {
  return settings_->value("facets_size_").toFloat();
}
/**
 * @brief Устанавливает толщину линий
 * @param width Толщина (в пикселях)
 */
void GLWidget::setLineWidth(float width) {
  settings_->setValue("facets_size_",
                      qBound(0.1f, width, 10.0f));  // Ограничиваем диапазон
  update();
}
/**
 * @brief Получить размер вершин модели.
 */
float GLWidget::getVertexSize() {
  return settings_->value("vertexes_size_").toFloat();
}
/**
 * @brief Устанавливает толщину вершин
 * @param width Толщина (в пикселях)
 */
void GLWidget::setVertexSize(float width) {
  settings_->setValue("vertexes_size_",
                      qBound(0.1f, width, 10.0f));  // Ограничиваем диапазон
  update();
}
/**
 * @brief Получить цвет фона модели.
 */
Colors GLWidget::getBackgroundColor() {
  Colors col;
  col.r = settings_->value("background_red_").toFloat();
  col.g = settings_->value("background_green_").toFloat();
  col.b = settings_->value("background_blue_").toFloat();
  return col;
}
/**
 * @brief Устанавливает цвет фона
 * @param r Красный (0.0 — 1.0)
 * @param g Зелёный (0.0 — 1.0)
 * @param b Синий (0.0 — 1.0)
 */
void GLWidget::setBackgroundColor(float r, float g, float b) {
  settings_->setValue("background_red_", r);
  settings_->setValue("background_green_", g);
  settings_->setValue("background_blue_", b);

  update();
}
/**
 * @brief Загрузить данные из конфига.
 */
void GLWidget::loadConfig() {
  settings_->value("vertexes_size_", defaultSize_).toInt();
  settings_->value("facets_size_", defaultSize_).toFloat();

  settings_->value("vertexes_red_", defaultColor2_).toFloat();
  settings_->value("vertexes_green_", defaultColor2_).toFloat();
  settings_->value("vertexes_blue_", defaultColor2_).toFloat();

  settings_->value("facets_red_", defaultColor2_).toFloat();
  settings_->value("facets_green_", defaultColor2_).toFloat();
  settings_->value("facets_blue_", defaultColor2_).toFloat();

  settings_->value("background_red_", defaultColor_).toFloat();
  settings_->value("background_green_", defaultColor_).toFloat();
  settings_->value("background_blue_", defaultColor_).toFloat();

  settings_->value("dotted_facets_", false).toBool();

  settings_->value("round_vertexes_", true).toBool();

  settings_->value("display_vertexes_", true).toBool();
  settings_->value("central_projection_", false).toBool();
}
/**
 * @brief Сохранить данные в конфиг.
 */
void GLWidget::saveConfig() {
  settings_->setValue("vertexes_size_",
                      settings_->value("vertexes_size_", defaultSize_));
  settings_->setValue("facets_size_",
                      settings_->value("facets_size_", defaultSize_));

  settings_->setValue("vertexes_red_",
                      settings_->value("vertexes_red_", defaultColor2_));
  settings_->setValue("vertexes_green_",
                      settings_->value("vertexes_green_", defaultColor2_));
  settings_->setValue("vertexes_blue_",
                      settings_->value("vertexes_blue_", defaultColor2_));

  settings_->setValue("facets_red_",
                      settings_->value("facets_red_", defaultColor2_));
  settings_->setValue("facets_green_",
                      settings_->value("facets_green_", defaultColor2_));
  settings_->setValue("facets_blue_",
                      settings_->value("facets_blue_", defaultColor2_));

  settings_->setValue("background_red_",
                      settings_->value("background_red_", defaultColor_));
  settings_->setValue("background_green_",
                      settings_->value("background_green_", defaultColor_));
  settings_->setValue("background_blue_",
                      settings_->value("background_blue_", defaultColor_));

  settings_->setValue("dotted_facets_",
                      settings_->value("dotted_facets_", false));
  settings_->setValue("round_vertexes_",
                      settings_->value("round_vertexes_", true));
  settings_->setValue("display_vertexes_",
                      settings_->value("display_vertexes_", true));
  settings_->setValue("central_projection_",
                      settings_->value("central_projection_", false));

  settings_->sync();
}