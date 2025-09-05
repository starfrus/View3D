#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(s21::Controller* controller, QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      controller_(controller)

{
  ui->setupUi(this);

  glWidget = new GLWidget(this);
  ui->visualizationLayout->addWidget(glWidget);

  // Устанавливаем соотношение: 1 часть — label, 9 частей — glWidget
  ui->visualizationLayout->setStretch(0, 1);  // visualizationLabel
  ui->visualizationLayout->setStretch(1, 9);  // glWidget

  // Установка начальных значений
  ui->translateXEdit->setText("0.0");
  ui->translateYEdit->setText("0.0");
  ui->translateZEdit->setText("0.0");
  ui->rotateXEdit->setText("0");
  ui->rotateYEdit->setText("0");
  ui->rotateZEdit->setText("0");
  ui->scaleFactorEdit->setText("1.00");
  ui->backgroundColorButton->setText("");
  ui->edgeColorButton->setText("");
  ui->vertexColorButton->setText("");

  setupConnections();
  updateInfoPanel();

  // Установить начальное состояние радиокнопок по значению из настроек
  if (glWidget->getCentralProjection()) {
    ui->perspectiveProjectionRadio->setChecked(true);
  } else {
    ui->parallelProjectionRadio->setChecked(true);
  }

  // Восстановить состояние рёбер из настроек
  ui->edgeWidthSpinBox->setValue(static_cast<int>(glWidget->getLineWidth()));

  int lineStyleIndex = glWidget->getDottedFacets() ? 1 : 0;
  ui->edgeLineStyleCombo->setCurrentIndex(lineStyleIndex);

  // Восстановить настройки вершин
  ui->vertexSizeSpinBox->setValue(static_cast<int>(glWidget->getVertexSize()));

  int styleIndex = glWidget->getRoundVertex() ? 0 : 1;  // 0 — круг, 1 — квадрат
  ui->vertexStyleCombo->setCurrentIndex(styleIndex);

  // Востановить цвет кнопок
  Colors lineColor = glWidget->getLineColor();
  Colors vertexColor = glWidget->getVertexColor();
  Colors bgColor = glWidget->getBackgroundColor();

  setButtonColor(ui->edgeColorButton,
                 QColor::fromRgbF(lineColor.r, lineColor.g, lineColor.b));
  setButtonColor(ui->vertexColorButton,
                 QColor::fromRgbF(vertexColor.r, vertexColor.g, vertexColor.b));
  setButtonColor(ui->backgroundColorButton,
                 QColor::fromRgbF(bgColor.r, bgColor.g, bgColor.b));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupConnections() {
  // Кнопка загрузки
  connect(ui->loadButton, &QPushButton::clicked, this,
          &MainWindow::onLoadButtonClicked);

  // Слайдеры перемещения
  connect(ui->translateXSlider, &QSlider::valueChanged, this,
          &MainWindow::onTranslateXSliderChanged);
  connect(ui->translateYSlider, &QSlider::valueChanged, this,
          &MainWindow::onTranslateYSliderChanged);
  connect(ui->translateZSlider, &QSlider::valueChanged, this,
          &MainWindow::onTranslateZSliderChanged);

  // Слайдеры поворота
  connect(ui->rotateXSlider, &QSlider::valueChanged, this,
          &MainWindow::onRotateXSliderChanged);
  connect(ui->rotateYSlider, &QSlider::valueChanged, this,
          &MainWindow::onRotateYSliderChanged);
  connect(ui->rotateZSlider, &QSlider::valueChanged, this,
          &MainWindow::onRotateZSliderChanged);

  // Слайдер масштаба
  connect(ui->scaleFactorSlider, &QSlider::valueChanged, this,
          &MainWindow::onScaleSliderChanged);

  // Редактирование текста -> обновление слайдера
  connect(ui->translateXEdit, &QLineEdit::editingFinished, this,
          &MainWindow::onTranslateXEditChanged);
  connect(ui->translateYEdit, &QLineEdit::editingFinished, this,
          &MainWindow::onTranslateYEditChanged);
  connect(ui->translateZEdit, &QLineEdit::editingFinished, this,
          &MainWindow::onTranslateZEditChanged);
  connect(ui->rotateXEdit, &QLineEdit::editingFinished, this,
          &MainWindow::onRotateXEditChanged);
  connect(ui->rotateYEdit, &QLineEdit::editingFinished, this,
          &MainWindow::onRotateYEditChanged);
  connect(ui->rotateZEdit, &QLineEdit::editingFinished, this,
          &MainWindow::onRotateZEditChanged);
  connect(ui->scaleFactorEdit, &QLineEdit::editingFinished, this,
          &MainWindow::onScaleFactorEditChanged);

  // Изменение цвета вершин, рёбер, фона
  connect(ui->edgeColorButton, &QPushButton::clicked, this,
          &MainWindow::onEdgeColorButtonClicked);

  connect(ui->vertexColorButton, &QPushButton::clicked, this,
          &MainWindow::onVertexColorButtonClicked);

  connect(ui->backgroundColorButton, &QPushButton::clicked, this,
          &MainWindow::onBackgroundColorButtonClicked);

  // Изменение типа проекции
  connect(ui->perspectiveProjectionRadio, &QRadioButton::toggled,
          [this](bool checked) {
            if (checked) {
              glWidget->setCentralProjection(true);  // центральная
              glWidget->update();
            }
          });

  connect(ui->parallelProjectionRadio, &QRadioButton::toggled,
          [this](bool checked) {
            if (checked) {
              glWidget->setCentralProjection(false);  // параллельная
              glWidget->update();
            }
          });

  // --- Настройка рёбер: тип линии (сплошная / пунктирная) ---
  connect(ui->edgeLineStyleCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index) {
            bool isDashed = (index == 1);  // 0 — сплошная, 1 — пунктирная
            glWidget->setDottedFacets(isDashed);
            glWidget->update();
          });

  // --- Настройка рёбер: толщина ---
  connect(ui->edgeWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          [this](int value) {
            glWidget->setLineWidth(static_cast<float>(value));
            glWidget->update();
          });

  // --- Настройка вершин: стиль (круг / квадрат) ---
  connect(ui->vertexStyleCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index) {
            bool isRound = (index == 0);  // 0 — круг, 1 — квадрат
            glWidget->setRoundVertex(isRound);
            glWidget->update();
          });

  // --- Настройка вершин: размер ---
  connect(ui->vertexSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
          [this](int value) {
            glWidget->setVertexSize(static_cast<float>(value));
            glWidget->update();
          });
}

void MainWindow::onLoadButtonClicked() {
  QString filePath =
      QFileDialog::getOpenFileName(this, tr("Открыть 3D-модель"), "",

                                   tr("3D файлы (*.obj *.OBJ);;Все файлы (*)"));

  if (filePath.isEmpty()) return;

  const bool ok = controller_->LoadModelFromFile(filePath.toStdString());

  if (ok) {
    auto* model = s21::ModelManager::GetInstance().GetModel();

    if (model) {
      ui->filePathEdit->setText(filePath);
      ui->visualizationLabel->setText("Модель загружена:\n" +
                                      QFileInfo(filePath).fileName());
      //   glWidget->setModelData(model->GetVertices(), model->GetEdges());
      glWidget->setModelData(&model->GetVertices(), model->GetEdges());

      updateInfoPanelFromModel();
    } else {
      auto* model = s21::ModelManager::GetInstance().GetModel();
      const QString err =
          model ? QString::fromStdString(model->GetLastErrorString())
                : tr("Не удалось загрузить модель");
      QMessageBox::warning(this, tr("Ошибка загрузки"), err);
      // очистим инфо-панель
      ui->filePathEdit->clear();

      updateInfoPanel();
    }
  }
}

void MainWindow::updateInfoPanel() {
  ui->infoFileName->setText("—");
  ui->infoVertices->setText("0");
  ui->infoEdges->setText("0");
}

void MainWindow::updateInfoPanelFromModel() {
  if (!controller_) return;

  int vertices = controller_->GetVerticesCount();
  int edges = controller_->GetEdgesCount();

  ui->infoVertices->setText(QString::number(vertices));
  ui->infoEdges->setText(QString::number(edges));

  // Если нет модели — очистим имя файла
  if (vertices == 0 && edges == 0) {
    ui->infoFileName->setText("—");
  } else {
    ui->infoFileName->setText(QFileInfo(ui->filePathEdit->text()).fileName());
  }
}

// --- Перемещение: слайдер -> поле ---
void MainWindow::onTranslateXSliderChanged(int value) {
  double val = value / 10.0;
  ui->translateXEdit->setText(QString::number(val, 'f', 1));
  double temp = val;
  val -= res_sdvigX_;
  res_sdvigX_ = temp;
  if (controller_) {
    controller_->TranslateModel(val, 0.0, 0.0);  // делаем через контроллер
    glWidget->update();                          // перерисовать
  }
}

void MainWindow::onTranslateYSliderChanged(int value) {
  double val = value / 10.0;
  ui->translateYEdit->setText(QString::number(val, 'f', 1));
  double temp = val;
  val -= res_sdvigY_;
  res_sdvigY_ = temp;

  if (controller_) {
    controller_->TranslateModel(0.0, val, 0.0);
    glWidget->update();
  }
}

void MainWindow::onTranslateZSliderChanged(int value) {
  double val = value / 10.0;
  ui->translateZEdit->setText(QString::number(val, 'f', 1));

  double temp = val;
  val -= res_sdvigZ_;
  res_sdvigZ_ = temp;
  if (controller_) {
    controller_->TranslateModel(0.0, 0.0, val);
    glWidget->update();
  }
}

// --- Поворот ---
void MainWindow::onRotateXSliderChanged(int value) {
  ui->rotateXEdit->setText(QString::number(value));

  double temp = value;
  value -= res_virtX_;
  res_virtX_ = temp;

  if (controller_) {
    controller_->RotateModel(value, 0.0, 0.0);
    glWidget->update();
  }
}

void MainWindow::onRotateYSliderChanged(int value) {
  ui->rotateYEdit->setText(QString::number(value));

  double temp = value;
  value -= res_virtY_;
  res_virtY_ = temp;

  if (controller_) {
    controller_->RotateModel(0.0, value, 0.0);
    glWidget->update();
  }
}

void MainWindow::onRotateZSliderChanged(int value) {
  ui->rotateZEdit->setText(QString::number(value));

  double temp = value;
  value -= res_virtZ_;
  res_virtZ_ = temp;

  if (controller_) {
    controller_->RotateModel(0.0, 0.0, value);
    glWidget->update();
  }
}

// --- Масштаб ---
void MainWindow::onScaleSliderChanged(int value) {
  double val = value / 100.0;  // абсолютное значение слайдера
  ui->scaleFactorEdit->setText(QString::number(val, 'f', 2));

  double factor = 1.0;
  if (res_scale_ != 0.0) {
    factor = val / res_scale_;  // вычисляем приращение
  }
  res_scale_ = val;  // сохраняем новое состояние

  if (controller_) {
    controller_->ScaleModel(factor);  // масштабируем на приращение
    glWidget->update();
  }
}

// --- Редактирование текста: поле -> слайдер ---

void MainWindow::onTranslateXEditChanged() {
  bool ok;
  double val = ui->translateXEdit->text().toDouble(&ok);
  if (ok && val >= -10.0 && val <= 10.0) {
    ui->translateXSlider->setValue(static_cast<int>(val * 10));
  } else {
    ui->translateXEdit->setText(
        QString::number(ui->translateXSlider->value() / 10.0, 'f', 1));
  }
}

void MainWindow::onTranslateYEditChanged() {
  bool ok;
  double val = ui->translateYEdit->text().toDouble(&ok);
  if (ok && val >= -10.0 && val <= 10.0) {
    ui->translateYSlider->setValue(static_cast<int>(val * 10));
  } else {
    ui->translateYEdit->setText(
        QString::number(ui->translateYSlider->value() / 10.0, 'f', 1));
  }
}

void MainWindow::onTranslateZEditChanged() {
  bool ok;
  double val = ui->translateZEdit->text().toDouble(&ok);
  if (ok && val >= -10.0 && val <= 10.0) {
    ui->translateZSlider->setValue(static_cast<int>(val * 10));
  } else {
    ui->translateZEdit->setText(
        QString::number(ui->translateZSlider->value() / 10.0, 'f', 1));
  }
}

void MainWindow::onRotateXEditChanged() {
  bool ok;
  int val = ui->rotateXEdit->text().toInt(&ok);
  if (ok && val >= 0 && val <= 360) {
    ui->rotateXSlider->setValue(val);
  } else {
    ui->rotateXEdit->setText(QString::number(ui->rotateXSlider->value()));
  }
}

void MainWindow::onRotateYEditChanged() {
  bool ok;
  int val = ui->rotateYEdit->text().toInt(&ok);
  if (ok && val >= 0 && val <= 360) {
    ui->rotateYSlider->setValue(val);
  } else {
    ui->rotateYEdit->setText(QString::number(ui->rotateYSlider->value()));
  }
}

void MainWindow::onRotateZEditChanged() {
  bool ok;
  int val = ui->rotateZEdit->text().toInt(&ok);
  if (ok && val >= 0 && val <= 360) {
    ui->rotateZSlider->setValue(val);
  } else {
    ui->rotateZEdit->setText(QString::number(ui->rotateZSlider->value()));
  }
}

void MainWindow::onScaleFactorEditChanged() {
  bool ok;
  double val = ui->scaleFactorEdit->text().toDouble(&ok);
  if (ok && val >= 0.1 && val <= 10.0) {
    ui->scaleFactorSlider->setValue(static_cast<int>(val * 100));
  } else {
    ui->scaleFactorEdit->setText(
        QString::number(ui->scaleFactorSlider->value() / 100.0, 'f', 2));
  }
}

void MainWindow::onEdgeColorButtonClicked() {
  QColorDialog dialog(Qt::white, this);
  dialog.setWindowTitle("Цвет рёбер");
  dialog.setOption(QColorDialog::ShowAlphaChannel, false);

  if (dialog.exec() == QDialog::Accepted) {
    QColor color = dialog.currentColor();
    glWidget->setLineColor(color.redF(), color.greenF(), color.blueF());
    glWidget->update();
    setButtonColor(ui->edgeColorButton, color);  // Обновляем цвет кнопки
  }
}

void MainWindow::onVertexColorButtonClicked() {
  QColorDialog dialog(Qt::red, this);
  dialog.setWindowTitle("Цвет вершин");
  dialog.setOption(QColorDialog::ShowAlphaChannel, false);

  if (dialog.exec() == QDialog::Accepted) {
    QColor color = dialog.currentColor();
    glWidget->setVertexColor(color.redF(), color.greenF(), color.blueF());
    glWidget->update();
    setButtonColor(ui->vertexColorButton, color);
  }
}

void MainWindow::onBackgroundColorButtonClicked() {
  QColorDialog dialog(Qt::black, this);
  dialog.setWindowTitle("Цвет фона");
  dialog.setOption(QColorDialog::ShowAlphaChannel, false);

  if (dialog.exec() == QDialog::Accepted) {
    QColor color = dialog.currentColor();
    glWidget->setBackgroundColor(color.redF(), color.greenF(), color.blueF());
    glWidget->update();
    setButtonColor(ui->backgroundColorButton, color);
  }
}
