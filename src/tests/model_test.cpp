#include "../model/model.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

namespace s21 {

class ModelTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Создаем временные файлы для тестов
    CreateValidObjFile();
    CreateInvalidObjFile();
    CreateEmptyObjFile();
    CreateObjFileWithComments();
  }

  void TearDown() override {
    // Удаляем временные файлы после тестов
    std::remove(valid_file_.c_str());
    std::remove(invalid_file_.c_str());
    std::remove(empty_file_.c_str());
    std::remove(comments_file_.c_str());
  }

  void CreateValidObjFile() {
    valid_file_ = "valid_test.obj";
    std::ofstream out(valid_file_);
    out << "v 1.0 2.0 3.0\n";
    out << "v 4.0 5.0 6.0\n";
    out << "v 7.0 8.0 9.0\n";
    out << "f 1 2 3\n";
    out.close();
  }

  void CreateInvalidObjFile() {
    invalid_file_ = "invalid_test.obj";
    std::ofstream out(invalid_file_);
    out << "v 1.0 2.0\n";      // Недостаточно координат
    out << "v abc def ghi\n";  // Нечисловые значения
    out << "f 1 2 3 4\n";      // Нормальная грань
    out << "f a b c\n";        // Нечисловые индексы
    out << "f -1 2 3\n";       // Отрицательный индекс
    out << "f 1 2 3 4 5\n";  // Многоугольник с 5 вершинами
    out.close();
  }

  void CreateEmptyObjFile() {
    empty_file_ = "empty_test.obj";
    std::ofstream out(empty_file_);
    out.close();
  }

  void CreateObjFileWithComments() {
    comments_file_ = "comments_test.obj";
    std::ofstream out(comments_file_);
    out << "# Это комментарий\n";
    out << "v 1.0 2.0 3.0\n";
    out << "# Еще комментарий\n";
    out << "f 1 1 1\n";
    out.close();
  }

  std::string valid_file_;
  std::string invalid_file_;
  std::string empty_file_;
  std::string comments_file_;
  Model model_;
};

TEST_F(ModelTest, LoadValidFile) {
  EXPECT_TRUE(model_.LoadFromFile(valid_file_));
  EXPECT_EQ(model_.GetVertexCount(), 3);

  const auto& vertices = model_.GetVertices();
  ASSERT_EQ(vertices.size(), 3);
  EXPECT_FLOAT_EQ(vertices[0].x, 1.0f);
  EXPECT_FLOAT_EQ(vertices[0].y, 2.0f);
  EXPECT_FLOAT_EQ(vertices[0].z, 3.0f);

  const auto& polygons = model_.GetPolygons();
  ASSERT_EQ(polygons.size(), 1);
  ASSERT_EQ(polygons[0].vertex_indices.size(), 3);
  EXPECT_EQ(polygons[0].vertex_indices[0],
            0);  // OBJ индексы начинаются с 1, в модели с 0
  EXPECT_EQ(polygons[0].vertex_indices[1], 1);
  EXPECT_EQ(polygons[0].vertex_indices[2], 2);
}

TEST_F(ModelTest, LoadNonexistentFile) {
  EXPECT_FALSE(model_.LoadFromFile("nonexistent_file.obj"));
  EXPECT_EQ(model_.GetVertexCount(), 0);
  EXPECT_TRUE(model_.GetVertices().empty());
  EXPECT_TRUE(model_.GetPolygons().empty());
}

TEST_F(ModelTest, LoadEmptyFile) {
  EXPECT_FALSE(model_.LoadFromFile(empty_file_));
  EXPECT_EQ(model_.GetVertexCount(), 0);
  EXPECT_TRUE(model_.GetVertices().empty());
  EXPECT_TRUE(model_.GetPolygons().empty());
}

TEST_F(ModelTest, LoadInvalidFile) {
  EXPECT_FALSE(model_.LoadFromFile(invalid_file_));
  EXPECT_EQ(model_.GetVertexCount(), 0);
  EXPECT_TRUE(model_.GetVertices().empty());
  EXPECT_TRUE(model_.GetPolygons().empty());
}

TEST_F(ModelTest, LoadFileWithComments) {
  EXPECT_TRUE(model_.LoadFromFile(comments_file_));
  EXPECT_EQ(model_.GetVertexCount(), 1);

  const auto& vertices = model_.GetVertices();
  ASSERT_EQ(vertices.size(), 1);
  EXPECT_FLOAT_EQ(vertices[0].x, 1.0f);
  EXPECT_FLOAT_EQ(vertices[0].y, 2.0f);
  EXPECT_FLOAT_EQ(vertices[0].z, 3.0f);

  // Полигон с одинаковыми вершинами считается невалидным
  const auto& polygons = model_.GetPolygons();
  EXPECT_TRUE(polygons.empty());
}

TEST_F(ModelTest, GetEdges) {
  // Создаем специальный файл для теста рёбер
  std::string edges_file = "edges_test.obj";
  std::ofstream out(edges_file);
  out << "v 0 0 0\nv 1 0 0\nv 0 1 0\n";
  out << "f 1 2 3\n";
  out.close();

  EXPECT_TRUE(model_.LoadFromFile(edges_file));

  auto edges = model_.GetEdges();
  ASSERT_EQ(edges.size(), 3);

  // Проверяем наличие всех ожидаемых рёбер
  std::set<std::pair<size_t, size_t>> edge_set;
  for (const auto& edge : edges) {
    edge_set.insert(edge.first < edge.second
                        ? std::make_pair(edge.first, edge.second)
                        : std::make_pair(edge.second, edge.first));
  }

  EXPECT_TRUE(edge_set.count({0, 1}) > 0);
  EXPECT_TRUE(edge_set.count({1, 2}) > 0);
  EXPECT_TRUE(edge_set.count({0, 2}) > 0);

  std::remove(edges_file.c_str());
}

TEST_F(ModelTest, GetEdgesForComplexPolygon) {
  std::string complex_file = "complex_test.obj";
  std::ofstream out(complex_file);
  out << "v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\n";
  out << "f 1 2 3 4\n";
  out.close();

  EXPECT_TRUE(model_.LoadFromFile(complex_file));

  auto edges = model_.GetEdges();
  ASSERT_EQ(edges.size(), 4);

  // Проверяем наличие всех ожидаемых рёбер
  std::set<std::pair<size_t, size_t>> edge_set;
  for (const auto& edge : edges) {
    edge_set.insert(edge.first < edge.second
                        ? std::make_pair(edge.first, edge.second)
                        : std::make_pair(edge.second, edge.first));
  }

  EXPECT_TRUE(edge_set.count({0, 1}) > 0);
  EXPECT_TRUE(edge_set.count({1, 2}) > 0);
  EXPECT_TRUE(edge_set.count({2, 3}) > 0);
  EXPECT_TRUE(edge_set.count({0, 3}) > 0);

  std::remove(complex_file.c_str());
}

TEST_F(ModelTest, ErrorHandling) {
  // Несуществующий файл
  EXPECT_FALSE(model_.LoadFromFile("nonexistent.obj"));
  EXPECT_EQ(static_cast<int>(model_.GetLastError()),
            static_cast<int>(Model::ErrorCode::kFileOpenError));

  // Пустой файл
  EXPECT_FALSE(model_.LoadFromFile(empty_file_));
  EXPECT_EQ(static_cast<int>(model_.GetLastError()),
            static_cast<int>(Model::ErrorCode::kNoValidData));

  // Невалидные данные
  EXPECT_FALSE(model_.LoadFromFile(invalid_file_));
  EXPECT_EQ(static_cast<int>(model_.GetLastError()),
            static_cast<int>(Model::ErrorCode::kNoValidData));
}

// TEST_F(ModelTest, Transformations) {
//   EXPECT_TRUE(model_.LoadFromFile(valid_file_));

//   // Test translation
//   model_.Translate(1.0f, 2.0f, 3.0f);
//   const auto& vertices = model_.GetVertices();
//   EXPECT_FLOAT_EQ(vertices[0].x, 2.0f);
//   EXPECT_FLOAT_EQ(vertices[0].y, 4.0f);
//   EXPECT_FLOAT_EQ(vertices[0].z, 6.0f);

//   // Test scaling
//   model_.Scale(2.0f);
//   EXPECT_FLOAT_EQ(vertices[0].x, 4.0f);
//   EXPECT_FLOAT_EQ(vertices[0].y, 8.0f);
//   EXPECT_FLOAT_EQ(vertices[0].z, 12.0f);

//   // Test rotation
//   model_.Rotate(90.0f, 0.0f, 0.0f);  // Rotate 90 degrees around X axis
//                                      // Add proper checks for rotation
//                                      results
// }

TEST_F(ModelTest, Validation) {
  Model m;
  EXPECT_FALSE(m.IsValid());

  EXPECT_TRUE(model_.LoadFromFile(valid_file_));
  EXPECT_TRUE(model_.IsValid());
}

}  // namespace s21