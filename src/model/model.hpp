/**
 * @file model.hpp
 * @brief Заголовочный файл для класса Model, отвечающего за загрузку и хранение
 * 3D-модели.
 *
 * Реализует чтение модели из файла формата .obj, хранение вершин и полигонов,
 * проверку корректности данных и базовые операции над моделью.
 *
 * @authors lioncoco, starfrus, melonyna
 * @version 2.0
 * @date 2025
 */

#ifndef MODEL_HPP
#define MODEL_HPP

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace s21 {

/**
 * @struct Vertex
 * @brief Структура, представляющая трёхмерную точку (вершину).
 *
 * Используется для хранения координат x, y, z вершины модели.
 * Реализовано сравнение с плавающей точкой с учётом погрешности.
 */
struct Vertex {
  float x, y, z;

  /**
   * @brief Оператор равенства для сравнения двух вершин.
   *
   * @param other Другая вершина для сравнения.
   * @return true если координаты совпадают с точностью до 1e-6.
   */
  bool operator==(const Vertex& other) const {
    return std::abs(x - other.x) < 1e-6f && std::abs(y - other.y) < 1e-6f &&
           std::abs(z - other.z) < 1e-6f;
  }
};

/**
 * @struct Polygon
 * @brief Структура, представляющая полигон (многоугольник).
 *
 * Хранит индексы вершин, образующих полигон. Проверяется на корректность при
 * необходимости.
 */
struct Polygon {
  std::vector<size_t> vertex_indices;

  /**
   * @brief Проверяет, является ли полигон валидным.
   *
   * Полигон должен содержать не менее 3 уникальных вершин и все индексы должны
   * быть в допустимом диапазоне.
   *
   * @param max_vertex_index Максимальный допустимый индекс вершины.
   * @return true если полигон валиден, false в противном случае.
   */
  bool IsValid(size_t max_vertex_index) const {
    if (vertex_indices.size() < 3) return false;

    std::set<size_t> unique_indices;
    for (size_t idx : vertex_indices) {
      if (idx >= max_vertex_index) return false;
      unique_indices.insert(idx);
    }

    return unique_indices.size() >= 3;
  }
};

/**
 * @enum ErrorCode
 * @brief Перечисление кодов ошибок, возникающих при работе с моделью.
 *
 * Используется для возврата информации об ошибках при загрузке модели.
 */
class Model {
 public:
  enum class ErrorCode {
    kSuccess = 0,        ///< Успешная операция
    kFileOpenError = 1,  ///< Ошибка открытия файла
    kInvalidData = 2,  ///< Некорректные данные в файле
    kNoValidData = 3  ///< Нет валидных данных после парсинга
  };

  /**
   * @brief Загружает 3D-модель из файла формата .obj.
   *
   * @param path Путь к файлу с моделью.
   * @return true если загрузка успешна, false в случае ошибки.
   */
  bool LoadFromFile(const std::string& path);

  /**
   * @brief Возвращает последний код ошибки.
   *
   * @return Код ошибки типа ErrorCode.
   */
  ErrorCode GetLastError() const { return last_error_; }

  /**
   * @brief Возвращает строковое описание последней ошибки.
   *
   * @return Строка с сообщением об ошибке.
   */
  const std::string& GetLastErrorString() const { return last_error_str_; }

  // --- Getters ---

  /**
   * @brief Возвращает изменяемый вектор вершин.
   *
   * @return Ссылка на вектор вершин.
   */
  std::vector<Vertex>& GetMutableVertices() { return vertices_; }

  /**
   * @brief Возвращает константный вектор вершин.
   *
   * @return Константная ссылка на вектор вершин.
   */
  const std::vector<Vertex>& GetVertices() const { return vertices_; }

  /**
   * @brief Возвращает константный вектор полигонов.
   *
   * @return Константная ссылка на вектор полигонов.
   */
  const std::vector<Polygon>& GetPolygons() const { return polygons_; }

  /**
   * @brief Возвращает количество вершин в модели.
   *
   * @return Размер вектора вершин.
   */
  size_t GetVertexCount() const { return vertices_.size(); }

  /**
   * @brief Возвращает количество полигонов в модели.
   *
   * @return Размер вектора полигонов.
   */
  size_t GetPolygonCount() const { return polygons_.size(); }

  /**
   * @brief Возвращает количество рёбер в модели.
   *
   * Рёбра вычисляются как уникальные пары смежных вершин в полигонах.
   *
   * @return Количество рёбер.
   */
  size_t GetEdgeCount() const { return GetEdges().size(); }

  /**
   * @brief Возвращает список рёбер модели.
   *
   * Каждое ребро представлено парой индексов вершин.
   *
   * @return Вектор пар индексов вершин.
   */
  std::vector<std::pair<size_t, size_t>> GetEdges() const;

  /**
   * @brief Возвращает путь к файлу модели.
   *
   * @return Путь к файлу.
   */
  const std::string GetPathFile() const { return path_file_; }

  /**
   * @brief Проверяет, является ли модель валидной.
   *
   * Модель считается валидной, если содержит хотя бы одну вершину и один
   * валидный полигон.
   *
   * @return true если модель валидна, false иначе.
   */
  bool IsValid() const;

  /**
   * @brief Нормализует модель.
   *
   * Вычисляет центр масс и радиус модели, чтобы она была центрирована и
   * масштабирована.
   */
  void NormalizeModel();

 private:
  std::string path_file_;         ///< Путь к файлу модели
  std::vector<Vertex> vertices_;  ///< Список вершин модели
  std::vector<Polygon> polygons_;  ///< Список полигонов модели
  ErrorCode last_error_ = ErrorCode::kSuccess;  ///< Последняя ошибка
  std::string last_error_str_;  ///< Строка с описанием ошибки

  /**
   * @brief Очищает информацию об ошибках.
   */
  void ClearErrors() {
    last_error_ = ErrorCode::kSuccess;
    last_error_str_.clear();
  }

  /**
   * @brief Устанавливает новую ошибку.
   *
   * @param code Код ошибки.
   * @param message Сообщение об ошибке.
   */
  void SetError(ErrorCode code, const std::string& message) {
    last_error_ = code;
    last_error_str_ = message;
  }

  /**
   * @brief Парсит строку с данными вершины.
   *
   * @param line Строка из файла .obj, начинающаяся с 'v'.
   * @return true если парсинг успешен.
   */
  bool ParseVertex(const std::string& line);

  /**
   * @brief Парсит строку с данными полигонов.
   *
   * @param line Строка из файла .obj, начинающаяся с 'f'.
   * @return true если парсинг успешен.
   */
  bool ParsePolygon(const std::string& line);
};

}  // namespace s21

#endif  // MODEL_HPP