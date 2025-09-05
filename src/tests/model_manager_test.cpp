#include "../patterns/model_manager.hpp"

#include <gtest/gtest.h>

#include <filesystem>

#include "../patterns/command.hpp"

namespace s21 {

class ModelManagerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Создаем временный файл для тестов
    CreateTestObjFile();
  }

  void TearDown() override {
    // Удаляем временный файл
    std::remove(test_file_.c_str());
    // Очищаем модель после каждого теста
    ModelManager::GetInstance().LoadModel("");  // Сброс модели
  }

  void CreateTestObjFile() {
    test_file_ = "test_model.obj";
    std::ofstream out(test_file_);
    out << "v 1.0 0.0 0.0\n";
    out << "v 0.0 1.0 0.0\n";
    out << "v 0.0 0.0 1.0\n";
    out << "f 1 2 3\n";
    out.close();
  }

  std::string test_file_;
};

// Тесты для Singleton паттерна в ModelManager
TEST_F(ModelManagerTest, SingletonInstance) {
  ModelManager& manager1 = ModelManager::GetInstance();
  ModelManager& manager2 = ModelManager::GetInstance();

  // Проверяем, что это один и тот же объект
  EXPECT_EQ(&manager1, &manager2);
}

TEST_F(ModelManagerTest, NoCopyAllowed) {
  // Проверяем, что копирование запрещено
  // Этот тест проверяет только что код компилируется
  ModelManager& manager = ModelManager::GetInstance();
  (void)manager;  // Используем переменную чтобы избежать warning

  // Если раскомментировать, должно приводить к ошибке компиляции
  // ModelManager copy = manager; // Должно не компилироваться
  // ModelManager another; // Должно не компилироваться
}

// Тесты для управления моделью
TEST_F(ModelManagerTest, ModelLoading) {
  ModelManager& manager = ModelManager::GetInstance();

  // Загрузка валидной модели
  EXPECT_TRUE(manager.LoadModel(test_file_));
  EXPECT_NE(manager.GetModel(), nullptr);

  // Проверяем что модель действительно загрузилась
  EXPECT_EQ(manager.GetModel()->GetVertexCount(), 3);
}

// TEST_F(ModelManagerTest, ModelReplacement) {
//   ModelManager& manager = ModelManager::GetInstance();

//   // Загружаем первую модель
//   Model* first_model_ptr = nullptr;
//   EXPECT_TRUE(manager.LoadModel(test_file_));
//   first_model_ptr = manager.GetModel();

//   // Загружаем вторую модель (должна заменить первую)
//   EXPECT_TRUE(manager.LoadModel(test_file_));
//   EXPECT_EQ(manager.GetModel(), first_model_ptr);
// }

TEST_F(ModelManagerTest, InvalidModelLoading) {
  ModelManager& manager = ModelManager::GetInstance();

  // Попытка загрузки несуществующего файла
  EXPECT_FALSE(manager.LoadModel("nonexistent.obj"));
}

// Тесты для Command паттерна
TEST_F(ModelManagerTest, MoveCommandExecution) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModel(test_file_));

  const auto& vertices_before = manager.GetModel()->GetVertices();
  float x_before = vertices_before[0].x;

  // Выполняем команду перемещения
  MoveCommand move(1.0f, 0.0f, 0.0f);
  move.Execute();

  const auto& vertices_after = manager.GetModel()->GetVertices();
  EXPECT_FLOAT_EQ(vertices_after[0].x, x_before + 1.0f);
}

TEST_F(ModelManagerTest, RotateCommandExecution) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModel(test_file_));

  const auto& vertices_before = manager.GetModel()->GetVertices();
  float y_before = vertices_before[0].y;
  float z_before = vertices_before[0].z;

  // Выполняем команду вращения (90 градусов вокруг X)
  RotateCommand rotate(90.0f, 0.0f, 0.0f);
  rotate.Execute();

  const auto& vertices_after = manager.GetModel()->GetVertices();
  EXPECT_NEAR(vertices_after[0].y, -z_before, 1e-6);
  EXPECT_NEAR(vertices_after[0].z, y_before, 1e-6);
}

TEST_F(ModelManagerTest, ScaleCommandExecution) {
  ModelManager& manager = ModelManager::GetInstance();
  ASSERT_TRUE(manager.LoadModel(test_file_));

  const auto& vertices_before = manager.GetModel()->GetVertices();
  float x_before = vertices_before[0].x;

  // Выполняем команду масштабирования
  ScaleCommand scale(2.0f);
  scale.Execute();

  const auto& vertices_after = manager.GetModel()->GetVertices();
  EXPECT_FLOAT_EQ(vertices_after[0].x, x_before * 2.0f);
}

TEST_F(ModelManagerTest, CommandWithNoModel) {
  ModelManager& manager = ModelManager::GetInstance();
  manager.LoadModel("");  // Сбрасываем модель

  // Пытаемся выполнить команды без модели (не должно быть краша)
  MoveCommand move(1.0f, 0.0f, 0.0f);
  EXPECT_NO_THROW(move.Execute());

  RotateCommand rotate(90.0f, 0.0f, 0.0f);
  EXPECT_NO_THROW(rotate.Execute());

  ScaleCommand scale(2.0f);
  EXPECT_NO_THROW(scale.Execute());
}

// Тест для проверки, что команды работают с единственным экземпляром модели
TEST_F(ModelManagerTest, CommandsWorkWithSingleton) {
  ModelManager& manager1 = ModelManager::GetInstance();
  ASSERT_TRUE(manager1.LoadModel(test_file_));

  // Получаем "другой" экземпляр (на самом деле тот же самый)
  ModelManager& manager2 = ModelManager::GetInstance();

  // Выполняем команду через первый "экземпляр"
  const auto& vertices_before = manager1.GetModel()->GetVertices();
  float x_before = vertices_before[0].x;

  MoveCommand move(1.0f, 0.0f, 0.0f);
  move.Execute();

  // Проверяем через второй "экземпляр"
  const auto& vertices_after = manager2.GetModel()->GetVertices();
  EXPECT_FLOAT_EQ(vertices_after[0].x, x_before + 1.0f);
}

}  // namespace s21