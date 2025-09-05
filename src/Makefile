# **************************************************************************** #
#                                                                              #
#                                                                              #
#    Makefile                                                                  #
#                                                                              #
#    By: lioncoco, starfrus, melonyna                                          #
#                                                                              #
#    Started with passion, finished with pride                                 #
#    Happy coding & good luck!                                                 #
#                                                                              #
#    Team 3DViewer - School 21                                                 #
#    We are glad to share our work!                                            #
#                                                                              #
# **************************************************************************** #

.PHONY: all rebuild install uninstall clean clang_format clang_check dist dvi test valgrind get_packets gcov_report run

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror
GCOVFLAGS = --coverage -fprofile-arcs -ftest-coverage
TEST_CXXFLAGS = -Lgtest/lib -lgtest -lgtest_main -pthread

# Исходные файлы и тесты
SRC_BACK = controller/*.hpp \
        model/*.cpp \
		patterns/*.hpp 

TESTS = ./tests/*_test.cpp
EXECUTABLE_TEST = test_app

# Директории
BUILD_DIR = ./build
DIST_DIR = 3DViewer_v2.0
GCOV_DIR = gcov_report

# Поиск всех исходных файлов
SRC_FILES := $(shell find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" \))

# Добавление флагов для gcov
gcov_report: CXXFLAGS += $(GCOVFLAGS)

# **************************************************************************** #
#                                    ЦЕЛИ                                      #
# **************************************************************************** #

## all: Сборка и установка проекта
all: install

## rebuild: Пересборка проекта
rebuild:
	@echo "Пересборка проекта..."
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release ../ && cmake --build .

## install: Установка проекта (сборка с помощью CMake)
install: uninstall
	@echo "Установка проекта..."
	mkdir $(BUILD_DIR) && cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release ../ && cmake --build .

## uninstall: Удаление сборочной директории
uninstall:
	@echo "Удаление сборочной директории..."
	rm -rf $(BUILD_DIR)

## test: Запуск unit-тестов
test: clean
	@echo "Запуск unit-тестов..."
	$(CXX) $(CXXFLAGS) $(SRC_BACK) $(TESTS) $(TEST_CXXFLAGS) -o $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)
	rm -f $(EXECUTABLE_TEST)

## gcov_report: Генерация отчета о покрытии кода тестами
gcov_report: clean
	@echo "Генерация отчета о покрытии кода..."
	mkdir -p $(GCOV_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_BACK) $(TESTS) $(TEST_CXXFLAGS) -o $(EXECUTABLE_TEST)
	./$(EXECUTABLE_TEST)

	lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch
	lcov --remove coverage.info '*/*test*' --output-file coverage.info
	lcov --remove coverage.info '/*/*/c++/*' --output-file coverage.info
	genhtml coverage.info --output-directory $(GCOV_DIR)
	@echo "Отчет сохранен в $(GCOV_DIR)/index.html"
	rm -f $(EXECUTABLE_TEST)

## valgrind: Проверка утечек памяти с помощью Valgrind
valgrind: clean
	@echo "Проверка утечек памяти..."
	$(CXX) $(CXXFLAGS) $(SRC_BACK) $(TESTS) $(TEST_CXXFLAGS) -o $(EXECUTABLE_TEST)
	valgrind -s --leak-check=yes --track-origins=yes ./$(EXECUTABLE_TEST)
	rm -f $(EXECUTABLE_TEST)

## dvi: Генерация документации с помощью Doxygen
dvi:
	@echo "Генерация документации..."
	ls
	doxygen Doxyfile
	@echo "Документация создана в директории docs/html/"

# Открытие документации в браузере
dvi_show:
	firefox docs/html/index.html

## dist: Создание дистрибутива проекта (архив)
dist:
	@echo "Создание дистрибутива..."
	mkdir -p ../$(DIST_DIR)
	cp -r ../src ../$(DIST_DIR)
	zip -r $(DIST_DIR).zip ../$(DIST_DIR)
	rm -rf ../$(DIST_DIR)
	@echo "Дистрибутив сохранен как $(DIST_DIR).zip"

## clang_format: Форматирование кода с помощью clang-format
clang_format:
	@echo "Форматирование кода..."
	clang-format -i $(SRC_FILES)

## clang_check: Проверка форматирования кода
clang_check:
	@echo "Проверка форматирования кода..."
	clang-format -n $(SRC_FILES)

## get_packets: Установка необходимых пакетов
get_packets:
	@echo "Установка необходимых пакетов..."
	sudo apt-get update
	sudo apt-get install -y qt6-base-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev doxygen graphviz

## clean: Очистка проекта от временных файлов
clean: uninstall
	@echo "Очистка проекта..."
	rm -rf gcov_report *.ini
	rm -rf $(GCOV_DIR)
	rm -f *.gcda *.gcno
	rm -f $(EXECUTABLE_TEST) coverage.info a.out
	rm -rf ./docs
	rm -rf $(DIST_DIR).zip

## run: Запуск собранного приложения
run:
	@echo "Запуск приложения..."
	@./build/3DModelViewer

# **************************************************************************** #
#                              ВСПОМОГАТЕЛЬНЫЕ ЦЕЛИ                            #
# **************************************************************************** #

## help: Показать список доступных целей
help:
	@echo "Доступные цели:"
	@echo "  all           - Сборка и установка проекта"
	@echo "  install       - Установка проекта"
	@echo "  rebuild       - Пересборка проекта"
	@echo "  uninstall     - Удаление сборочной директории"
	@echo "  test          - Запуск unit-тестов"
	@echo "  gcov_report   - Генерация отчета о покрытии кода"
	@echo "  valgrind      - Проверка утечек памяти"
	@echo "  dvi           - Генерация документации"
	@echo "  dist          - Создание дистрибутива"
	@echo "  clang_format  - Форматирование кода"
	@echo "  clang_check   - Проверка форматирования"
	@echo "  get_packets   - Установка необходимых пакетов"
	@echo "  clean         - Очистка проекта"
	@echo "  run           - Запуск приложения"
	@echo "  help          - Показать это сообщение"