#include <gtest/gtest.h>

#include <cmath>
#include <fstream>

#include "../patterns/command.hpp"
#include "../patterns/model_manager.hpp"

const float TEST_EPSILON = 1e-5f;

constexpr float deg_to_rad = M_PI / 180.0f;

namespace s21 {

class AffineTransformTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Создаем временный файл с простой моделью
    test_file_ = "affine_test.obj";
    std::ofstream out(test_file_);
    out << "v 1.0 0.0 0.0\n";  // Вершина на оси X
    out << "v 0.0 1.0 0.0\n";  // Вершина на оси Y
    out << "v 0.0 0.0 1.0\n";  // Вершина на оси Z
    out << "f 1 2 3\n";
    out.close();
  }

  void TearDown() override {
    // Удаляем временный файл
    std::remove(test_file_.c_str());
    // Сбрасываем модель
    ModelManager::GetInstance().LoadModelForTest("");
  }

  std::string test_file_;
};

TEST_F(AffineTransformTest, CombinedTransformations) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  // Сохраняем оригинальные вершины
  const auto original_vertices = manager.GetModel()->GetVertices();
  ASSERT_EQ(original_vertices.size(), 3);

  // Параметры преобразований
  const float dx = 1.5f, dy = -2.0f, dz = 0.5f;
  const float angle_x = 45.0f, angle_y = 30.0f, angle_z = 15.0f;
  const float scale = 2.0f;

  // Применяем преобразования
  MoveCommand(dx, dy, dz).Execute();
  RotateCommand(angle_x, angle_y, angle_z).Execute();
  ScaleCommand(scale).Execute();

  // Проверяем результаты
  const auto& transformed = manager.GetModel()->GetVertices();

  // Для каждой вершины вычисляем ожидаемый результат
  for (size_t i = 0; i < original_vertices.size(); ++i) {
    Vertex expected = original_vertices[i];

    // 1. Применяем перемещение
    expected.x += dx;
    expected.y += dy;
    expected.z += dz;

    // 2. Применяем вращение (X -> Y -> Z)
    // Вращение вокруг X
    float y = expected.y;
    float z = expected.z;
    expected.y = y * cos(angle_x * deg_to_rad) - z * sin(angle_x * deg_to_rad);
    expected.z = y * sin(angle_x * deg_to_rad) + z * cos(angle_x * deg_to_rad);

    // Вращение вокруг Y
    float x = expected.x;
    z = expected.z;
    expected.x = x * cos(angle_y * deg_to_rad) + z * sin(angle_y * deg_to_rad);
    expected.z = -x * sin(angle_y * deg_to_rad) + z * cos(angle_y * deg_to_rad);

    // Вращение вокруг Z
    x = expected.x;
    y = expected.y;
    expected.x = x * cos(angle_z * deg_to_rad) - y * sin(angle_z * deg_to_rad);
    expected.y = x * sin(angle_z * deg_to_rad) + y * cos(angle_z * deg_to_rad);

    // 3. Применяем масштабирование
    expected.x *= scale;
    expected.y *= scale;
    expected.z *= scale;

    // Сравниваем с фактическим результатом
    EXPECT_NEAR(transformed[i].x, expected.x, TEST_EPSILON);
    EXPECT_NEAR(transformed[i].y, expected.y, TEST_EPSILON);
    EXPECT_NEAR(transformed[i].z, expected.z, TEST_EPSILON);
  }
}

TEST_F(AffineTransformTest, TransformationOrder) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  // Проверяем порядок выполнения операций (сначала вращение, потом перемещение)
  RotateCommand(0.0f, 90.0f, 0.0f).Execute();  // Вращаем на 90° вокруг Y
  MoveCommand(1.0f, 0.0f, 0.0f).Execute();  // Перемещаем по X

  const auto& vertices = manager.GetModel()->GetVertices();
  ASSERT_EQ(vertices.size(), 3);

  // Проверяем первую вершину (изначально 1,0,0)
  const auto& v = vertices[0];

  // После вращения (1,0,0) -> (0,0,-1)
  // После перемещения -> (1,0,-1)
  EXPECT_NEAR(v.x, 1.0f, TEST_EPSILON);
  EXPECT_NEAR(v.y, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(v.z, -1.0f, TEST_EPSILON);
}

TEST_F(AffineTransformTest, RotateXTest) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  RotateCommand(90.0f, 0.0f, 0.0f).Execute();  // Вращаем на 90° вокруг X

  const auto& vertices = manager.GetModel()->GetVertices();
  ASSERT_EQ(vertices.size(), 3);

  // Проверяем вторую вершину (изначально 0,1,0)
  const auto& v = vertices[1];

  // После вращения (0,1,0) -> (0,0,1)
  EXPECT_NEAR(v.x, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(v.y, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(v.z, 1.0f, TEST_EPSILON);
}

TEST_F(AffineTransformTest, RotateYTest) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  RotateCommand(0.0f, 90.0f, 0.0f).Execute();  // Вращаем на 90° вокруг Y

  const auto& vertices = manager.GetModel()->GetVertices();
  ASSERT_EQ(vertices.size(), 3);

  // Проверяем третью вершину (изначально 0,0,1)
  const auto& v = vertices[2];

  // После вращения (0,0,1) -> (1,0,0)
  EXPECT_NEAR(v.x, 1.0f, TEST_EPSILON);
  EXPECT_NEAR(v.y, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(v.z, 0.0f, TEST_EPSILON);
}

TEST_F(AffineTransformTest, RotateZTest) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  RotateCommand(0.0f, 0.0f, 90.0f).Execute();  // Вращаем на 90° вокруг Z

  const auto& vertices = manager.GetModel()->GetVertices();
  ASSERT_EQ(vertices.size(), 3);

  // Проверяем первую вершину (изначально 1,0,0)
  const auto& v = vertices[0];

  // После вращения (1,0,0) -> (0,1,0)
  EXPECT_NEAR(v.x, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(v.y, 1.0f, TEST_EPSILON);
  EXPECT_NEAR(v.z, 0.0f, TEST_EPSILON);
}

TEST_F(AffineTransformTest, MoveTest) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  MoveCommand(2.0f, -1.0f, 0.5f).Execute();

  const auto& vertices = manager.GetModel()->GetVertices();
  ASSERT_EQ(vertices.size(), 3);

  // Проверяем все вершины
  EXPECT_NEAR(vertices[0].x, 3.0f, TEST_EPSILON);   // 1+2
  EXPECT_NEAR(vertices[0].y, -1.0f, TEST_EPSILON);  // 0-1
  EXPECT_NEAR(vertices[0].z, 0.5f, TEST_EPSILON);   // 0+0.5

  EXPECT_NEAR(vertices[1].x, 2.0f, TEST_EPSILON);  // 0+2
  EXPECT_NEAR(vertices[1].y, 0.0f, TEST_EPSILON);  // 1-1
  EXPECT_NEAR(vertices[1].z, 0.5f, TEST_EPSILON);  // 0+0.5

  EXPECT_NEAR(vertices[2].x, 2.0f, TEST_EPSILON);   // 0+2
  EXPECT_NEAR(vertices[2].y, -1.0f, TEST_EPSILON);  // 0-1
  EXPECT_NEAR(vertices[2].z, 1.5f, TEST_EPSILON);   // 1+0.5
}

TEST_F(AffineTransformTest, ScaleTest) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  ScaleCommand(2.5f).Execute();

  const auto& vertices = manager.GetModel()->GetVertices();
  ASSERT_EQ(vertices.size(), 3);

  // Проверяем масштабирование
  EXPECT_NEAR(vertices[0].x, 2.5f, TEST_EPSILON);  // 1.0 * 2.5
  EXPECT_NEAR(vertices[0].y, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(vertices[0].z, 0.0f, TEST_EPSILON);

  EXPECT_NEAR(vertices[1].x, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(vertices[1].y, 2.5f, TEST_EPSILON);  // 1.0 * 2.5
  EXPECT_NEAR(vertices[1].z, 0.0f, TEST_EPSILON);

  EXPECT_NEAR(vertices[2].x, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(vertices[2].y, 0.0f, TEST_EPSILON);
  EXPECT_NEAR(vertices[2].z, 2.5f, TEST_EPSILON);  // 1.0 * 2.5
}

TEST_F(AffineTransformTest, CombinedTransformations2) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  // Применяем несколько преобразований
  RotateCommand(0.0f, 90.0f, 0.0f).Execute();  // Вращение вокруг Y
  MoveCommand(1.0f, 2.0f, 3.0f).Execute();     // Перемещение
  ScaleCommand(2.0f).Execute();                // Масштабирование

  const auto& vertices = manager.GetModel()->GetVertices();
  ASSERT_EQ(vertices.size(), 3);

  // Проверяем первую вершину (изначально 1,0,0)
  // После вращения: (0,0,-1)
  // После перемещения: (1,2,2)
  // После масштабирования: (2,4,4)
  EXPECT_NEAR(vertices[0].x, 2.0f, TEST_EPSILON);
  EXPECT_NEAR(vertices[0].y, 4.0f, TEST_EPSILON);
  EXPECT_NEAR(vertices[0].z, 4.0f, TEST_EPSILON);
}

TEST_F(AffineTransformTest, ZeroScaleHandling) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModelForTest(test_file_));

  const auto original = manager.GetModel()->GetVertices();
  ScaleCommand(0.0f).Execute();  // Масштабирование на 0

  // Проверяем что модель не изменилась (или обработала специальным образом)
  const auto& scaled = manager.GetModel()->GetVertices();
  for (size_t i = 0; i < original.size(); ++i) {
    EXPECT_NEAR(scaled[i].x, original[i].x, TEST_EPSILON);
    EXPECT_NEAR(scaled[i].y, original[i].y, TEST_EPSILON);
    EXPECT_NEAR(scaled[i].z, original[i].z, TEST_EPSILON);
  }
}

}  // namespace s21