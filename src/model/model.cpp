#include "model.hpp"

namespace s21 {

bool Model::LoadFromFile(const std::string& path) {
  ClearErrors();
  vertices_.clear();
  polygons_.clear();
  path_file_ = path;

  std::ifstream file(path);
  if (!file.is_open()) {
    SetError(ErrorCode::kFileOpenError, "Failed to open file: " + path);
    path_file_ = "Failed to open file: " + path;
    return false;
  }

  std::string line;
  size_t line_num = 0;
  bool has_valid_data = false;

  while (std::getline(file, line)) {
    line_num++;
    line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
                 return !std::isspace(ch);
               }));

    if (line.empty() || line[0] == '#') {
      continue;
    }

    try {
      if (line.starts_with("v ")) {
        if (ParseVertex(line)) {
          has_valid_data = true;
        }
      } else if (line.starts_with("f ")) {
        if (ParsePolygon(line)) {
          has_valid_data = true;
        }
      }
    } catch (const std::exception& e) {
      SetError(ErrorCode::kInvalidData,
               "Error at line " + std::to_string(line_num) + ": " + e.what());
    }
  }

  if (!has_valid_data) {
    SetError(ErrorCode::kNoValidData, "No valid data found in file");
    return false;
  }

  if (!IsValid()) {
    SetError(ErrorCode::kInvalidData, "Loaded data is invalid");
    return false;
  }

  return true;
}

bool Model::ParseVertex(const std::string& line) {
  Vertex v;
  int count = sscanf(line.c_str(), "v %f %f %f", &v.x, &v.y, &v.z);
  if (count != 3) {
    throw std::runtime_error("Invalid vertex format");
  }
  vertices_.push_back(v);
  return true;
}

bool Model::ParsePolygon(const std::string& line) {
  Polygon p;
  std::istringstream iss(line.substr(2));
  std::string token;

  while (iss >> token) {
    size_t pos = token.find('/');
    if (pos != std::string::npos) {
      token = token.substr(0, pos);
    }

    try {
      size_t idx = std::stoul(token);
      if (idx == 0 || idx > vertices_.size()) {
        throw std::out_of_range("Vertex index out of range");
      }
      p.vertex_indices.push_back(idx - 1);
    } catch (...) {
      throw std::runtime_error("Invalid face index: " + token);
    }
  }

  if (p.IsValid(vertices_.size())) {
    polygons_.push_back(p);
    return true;
  }
  return false;
}

std::vector<std::pair<size_t, size_t>> Model::GetEdges() const {
  std::set<std::pair<size_t, size_t>> unique_edges;

  for (const Polygon& poly : polygons_) {
    if (poly.vertex_indices.size() < 2) continue;

    for (size_t i = 0; i < poly.vertex_indices.size(); ++i) {
      size_t start = poly.vertex_indices[i];
      size_t end = poly.vertex_indices[(i + 1) % poly.vertex_indices.size()];

      if (start == end) continue;

      auto edge = (start < end) ? std::make_pair(start, end)
                                : std::make_pair(end, start);
      unique_edges.insert(edge);
    }
  }

  return {unique_edges.begin(), unique_edges.end()};
}

bool Model::IsValid() const {
  if (vertices_.empty()) return false;

  for (const Polygon& poly : polygons_) {
    if (!poly.IsValid(vertices_.size())) {
      return false;
    }
  }

  return true;
}

void Model::NormalizeModel() {
  if (vertices_.empty()) {
    return;  // Нечего нормализовать
  }

  // Шаг 1: Находим bounding box (min и max по осям)
  float min_x = vertices_[0].x, min_y = vertices_[0].y, min_z = vertices_[0].z;
  float max_x = vertices_[0].x, max_y = vertices_[0].y, max_z = vertices_[0].z;

  for (const auto& v : vertices_) {
    min_x = std::min(min_x, v.x);
    min_y = std::min(min_y, v.y);
    min_z = std::min(min_z, v.z);

    max_x = std::max(max_x, v.x);
    max_y = std::max(max_y, v.y);
    max_z = std::max(max_z, v.z);
  }

  // Шаг 2: Вычисляем центр габаритного объёма
  float center_x = (min_x + max_x) * 0.5f;
  float center_y = (min_y + max_y) * 0.5f;
  float center_z = (min_z + max_z) * 0.5f;

  // Шаг 3: Вычисляем половину диагонали (радиус описанной сферы)
  float dx = max_x - min_x;
  float dy = max_y - min_y;
  float dz = max_z - min_z;
  float diagonal = std::sqrt(dx * dx + dy * dy + dz * dz);
  float radius = diagonal * 0.5f;

  // Защита от деления на ноль (все точки совпадают)
  if (radius < 1e-6f) {
    radius = 1.0f;
  }

  // Шаг 4: Нормализуем все вершины
  // 1. Сдвигаем, чтобы центр был в (0, 0, 0)
  // 2. Масштабируем, чтобы модель вписалась в сферу радиуса 1.0
  float scale_factor = 1.0f / radius;

  for (auto& v : vertices_) {
    v.x = (v.x - center_x) * scale_factor;
    v.y = (v.y - center_y) * scale_factor;
    v.z = (v.z - center_z) * scale_factor;
  }
}

}  // namespace s21