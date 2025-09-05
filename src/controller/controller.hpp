/**
 * @file controller.hpp
 * @brief Заголовочный файл для контроллера приложения 3DViewer.
 *
 * Содержит объявление класса Controller - основного управляющего компонента
 * приложения, обеспечивающего связь между GUI и бизнес-логикой.
 *
 * @authors lioncoco, starfrus, melonyna
 * @version 1.0
 * @date 2025
 */

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <string>

#include "../patterns/command.hpp"
#include "../patterns/model_manager.hpp"

namespace s21 {

/**
 * @class Controller
 * @brief Контроллер приложения 3DViewer.
 *
 * Реализует паттерн Controller из архитектуры MVC. Отвечает за:
 * - Загрузку 3D-моделей
 * - Получение информации о модели
 * - Выполнение трансформаций модели через паттерн Command
 * - Обработку ошибок
 */
class Controller {
 public:
  /**
   * @brief Конструктор контроллера.
   *
   * @param model_manager Ссылка на менеджер моделей (Singleton).
   */
  explicit Controller(ModelManager& model_manager)
      : model_manager_(model_manager) {}

  /**
   * @brief Загружает 3D-модель из файла.
   *
   * Делегирует загрузку ModelManager. После успешной загрузки
   * модель автоматически нормализуется.
   *
   * @param path Путь к файлу модели (.obj).
   * @return true если загрузка успешна, false в случае ошибки.
   */
  bool LoadModelFromFile(const std::string& path) {
    return model_manager_.LoadModel(path);
  }

  /**
   * @brief Получает количество вершин в текущей модели.
   *
   * @return Количество вершин или 0, если модель не загружена.
   */
  int GetVerticesCount() const {
    auto* model = model_manager_.GetModel();
    return model ? model->GetVertexCount() : 0;
  }

  /**
   * @brief Получает количество рёбер в текущей модели.
   *
   * @return Количество рёбер или 0, если модель не загружена.
   */
  int GetEdgesCount() const {
    auto* model = model_manager_.GetModel();
    return model ? model->GetEdgeCount() : 0;
  }

  /**
   * @brief Получает строку с описанием последней ошибки.
   *
   * @return Строка с описанием ошибки или сообщение о незагруженной модели.
   */
  std::string GetLastErrorString() const {
    auto* model = model_manager_.GetModel();
    return model ? model->GetLastErrorString() : "Модель не загружена";
  }

  /**
   * @brief Выполняет перемещение модели.
   *
   * Создаёт и выполняет команду перемещения через паттерн Command.
   *
   * @param dx Смещение по оси X.
   * @param dy Смещение по оси Y.
   * @param dz Смещение по оси Z.
   */
  void TranslateModel(double dx, double dy, double dz) {
    s21::MoveCommand cmd(dx, dy, dz);
    cmd.Execute();
  }

  /**
   * @brief Выполняет поворот модели.
   *
   * Создаёт и выполняет команду поворота через паттерн Command.
   *
   * @param angle_x Угол поворота вокруг оси X в градусах.
   * @param angle_y Угол поворота вокруг оси Y в градусах.
   * @param angle_z Угол поворота вокруг оси Z в градусах.
   */
  void RotateModel(double angle_x, double angle_y, double angle_z) {
    RotateCommand cmd(angle_x, angle_y, angle_z);
    cmd.Execute();
  }

  /**
   * @brief Выполняет масштабирование модели.
   *
   * Создаёт и выполняет команду масштабирования через паттерн Command.
   *
   * @param factor Коэффициент масштабирования.
   */
  void ScaleModel(double factor) {
    ScaleCommand cmd(factor);
    cmd.Execute();
  }

 private:
  ModelManager& model_manager_;  ///< Ссылка на менеджер моделей (Singleton)
};

}  // namespace s21

#endif  // CONTROLLER_HPP