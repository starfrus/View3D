/**
 * @file model_manager.hpp
 * @brief Заголовочный файл для класса ModelManager - менеджера 3D-моделей.
 *
 * Реализует паттерн Singleton для управления единственным экземпляром 3D-модели
 * в приложении. Обеспечивает централизованный доступ к текущей модели и её
 * загрузку.
 *
 * @authors lioncoco, starfrus, melonyna
 * @version 1.0
 * @date 2025
 */

#ifndef MODEL_MANAGER_HPP
#define MODEL_MANAGER_HPP

#include "../model/model.hpp"

namespace s21 {

/**
 * @class ModelManager
 * @brief Класс-менеджер для управления единственным экземпляром 3D-модели.
 *
 * Реализует паттерн Singleton, гарантируя существование только одного
 * экземпляра менеджера в приложении. Отвечает за загрузку, хранение и доступ к
 * текущей модели.
 */
class ModelManager {
 public:
  /**
   * @brief Удалённый конструктор копирования.
   *
   * Предотвращает создание копий менеджера.
   */
  ModelManager(const ModelManager&) = delete;

  /**
   * @brief Удалённый оператор присваивания.
   *
   * Предотвращает присваивание одного менеджера другому.
   */
  void operator=(const ModelManager&) = delete;

  /**
   * @brief Получает ссылку на единственный экземпляр ModelManager.
   *
   * Реализует ленивую инициализацию (lazy initialization) - экземпляр создаётся
   * при первом вызове метода.
   *
   * @return Ссылка на единственный экземпляр ModelManager.
   */
  static ModelManager& GetInstance() {
    static ModelManager instance;
    return instance;
  }

  /**
   * @brief Получает указатель на текущую модель.
   *
   * @return Указатель на текущую модель или nullptr, если модель не загружена.
   */
  Model* GetModel() const { return current_model_; }

  /**
   * @brief Загружает модель из файла и нормализует её.
   *
   * Предварительно удаляет предыдущую модель, если она существовала.
   * После успешной загрузки автоматически нормализует модель.
   *
   * @param path Путь к файлу модели (.obj).
   * @return true если загрузка успешна, false в случае ошибки.
   */
  bool LoadModel(const std::string& path) {
    delete current_model_;
    current_model_ = new Model();
    bool flag = current_model_->LoadFromFile(path);
    if (flag) {
      current_model_->NormalizeModel();
    }
    return flag;
  }

  /**
   * @brief Загружает модель из файла без нормализации (для тестирования).
   *
   * Предварительно удаляет предыдущую модель, если она существовала.
   * Используется в тестах, где нормализация не требуется.
   *
   * @param path Путь к файлу модели (.obj).
   * @return true если загрузка успешна, false в случае ошибки.
   */
  bool LoadModelForTest(const std::string& path) {
    delete current_model_;
    current_model_ = new Model();
    bool flag = current_model_->LoadFromFile(path);

    return flag;
  }

 private:
  Model* current_model_ = nullptr;  ///< Указатель на текущую загруженную модель

  /**
   * @brief Приватный конструктор.
   *
   * Предотвращает создание экземпляра извне. Используется только внутри
   * GetInstance().
   */
  ModelManager() = default;

  /**
   * @brief Приватный деструктор.
   *
   * Автоматически удаляет текущую модель при уничтожении менеджера.
   */
  ~ModelManager() { delete current_model_; }
};

}  // namespace s21

#endif  // MODEL_MANAGER_HPP