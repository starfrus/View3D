/**
 * @file command.hpp
 * @brief Заголовочный файл для реализации паттерна Command.
 *
 * Содержит абстрактный класс Command и его конкретные реализации:
 * MoveCommand, RotateCommand, ScaleCommand для выполнения трансформаций над
 * 3D-моделью.
 *
 * @authors lioncoco, starfrus, melonyna
 * @version 1.0
 * @date 2025
 */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "model_manager.hpp"

namespace s21 {

/**
 * @class Command
 * @brief Абстрактный базовый класс для команд трансформации модели.
 *
 * Реализует паттерн Command, позволяющий инкапсулировать операции над моделью
 * в виде объектов. Поддерживает отмену операций и составные команды.
 */
class Command {
 public:
  /**
   * @brief Виртуальный деструктор.
   *
   * Обеспечивает правильное удаление производных классов.
   */
  virtual ~Command() = default;

  /**
   * @brief Выполняет команду.
   *
   * Чисто виртуальный метод, который должен быть реализован в производных
   * классах.
   */
  virtual void Execute() = 0;
};

/**
 * @class MoveCommand
 * @brief Команда перемещения модели вдоль осей X, Y, Z.
 *
 * Реализует линейное смещение всех вершин модели на заданные значения.
 */
class MoveCommand : public Command {
 public:
  /**
   * @brief Конструктор команды перемещения.
   *
   * @param dx Смещение по оси X.
   * @param dy Смещение по оси Y.
   * @param dz Смещение по оси Z.
   */
  MoveCommand(float dx, float dy, float dz) : dx_(dx), dy_(dy), dz_(dz) {}

  /**
   * @brief Выполняет перемещение модели.
   *
   * Прибавляет заданные значения ко всем координатам вершин текущей модели.
   */
  void Execute() override {
    auto& manager = ModelManager::GetInstance();

    if (auto* model = manager.GetModel()) {
      auto& vertices = model->GetMutableVertices();
      for (auto& vertex : vertices) {
        vertex.x += dx_;
        vertex.y += dy_;
        vertex.z += dz_;
      }
    }
  }

 private:
  float dx_;  ///< Смещение по оси X
  float dy_;  ///< Смещение по оси Y
  float dz_;  ///< Смещение по оси Z
};

/**
 * @class RotateCommand
 * @brief Команда вращения модели вокруг осей X, Y, Z.
 *
 * Реализует поворот модели на заданные углы вокруг каждой из осей.
 */
class RotateCommand : public Command {
 public:
  /**
   * @brief Конструктор команды вращения.
   *
   * @param angle_x Угол поворота вокруг оси X в градусах.
   * @param angle_y Угол поворота вокруг оси Y в градусах.
   * @param angle_z Угол поворота вокруг оси Z в градусах.
   */
  RotateCommand(float angle_x, float angle_y, float angle_z)
      : angle_x_(angle_x), angle_y_(angle_y), angle_z_(angle_z) {}

  /**
   * @brief Выполняет вращение модели.
   *
   * Применяет матричные преобразования для поворота всех вершин модели.
   */
  void Execute() override {
    auto& manager = ModelManager::GetInstance();
    if (auto* model = manager.GetModel()) {
      RotateVertices(model->GetMutableVertices());
    }
  }

 private:
  /**
   * @brief Применяет вращение ко всем вершинам модели.
   *
   * @param vertices Вектор вершин для трансформации.
   * @param inverse Флаг обратного вращения (для отмены операций).
   */
  void RotateVertices(std::vector<Vertex>& vertices, bool inverse = false) {
    const float deg_to_rad = M_PI / 180.0f;
    float ax = angle_x_ * deg_to_rad * (inverse ? -1 : 1);
    float ay = angle_y_ * deg_to_rad * (inverse ? -1 : 1);
    float az = angle_z_ * deg_to_rad * (inverse ? -1 : 1);

    for (auto& vertex : vertices) {
      if (ax != 0) RotatePair(vertex.y, vertex.z, ax);
      if (ay != 0) RotatePair(vertex.z, vertex.x, ay);
      if (az != 0) RotatePair(vertex.x, vertex.y, az);
    }
  }

  /**
   * @brief Поворачивает пару координат на заданный угол.
   *
   * Использует формулы вращения в двумерной плоскости.
   *
   * @param coord1 Первая координата (например, x).
   * @param coord2 Вторая координата (например, y).
   * @param angle Угол поворота в радианах.
   */
  void RotatePair(float& coord1, float& coord2, float angle) {
    float p1 = coord1 * cos(angle) - coord2 * sin(angle);
    float p2 = coord1 * sin(angle) + coord2 * cos(angle);
    coord1 = p1;
    coord2 = p2;
  }

  float angle_x_;  ///< Угол поворота вокруг оси X
  float angle_y_;  ///< Угол поворота вокруг оси Y
  float angle_z_;  ///< Угол поворота вокруг оси Z
};

/**
 * @class ScaleCommand
 * @brief Команда масштабирования модели.
 *
 * Реализует равномерное масштабирование модели относительно центра координат.
 */
class ScaleCommand : public Command {
 public:
  /**
   * @brief Конструктор команды масштабирования.
   *
   * @param factor Коэффициент масштабирования (должен быть положительным).
   */
  ScaleCommand(float factor) : factor_(factor) {}

  /**
   * @brief Выполняет масштабирование модели.
   *
   * Умножает координаты всех вершин на коэффициент масштабирования.
   */
  void Execute() override {
    if (factor_ <= 0.0f) return;
    auto& manager = ModelManager::GetInstance();
    if (auto* model = manager.GetModel()) {
      auto& vertices = model->GetMutableVertices();
      for (auto& vertex : vertices) {
        vertex.x *= factor_;
        vertex.y *= factor_;
        vertex.z *= factor_;
      }
    }
  }

 private:
  float factor_;  ///< Коэффициент масштабирования
};

}  // namespace s21

#endif  // COMMAND_HPP