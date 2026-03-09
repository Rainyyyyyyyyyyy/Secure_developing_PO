#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <Windows.h>

namespace fs = std::filesystem;

// Рекурсивная функция для обхода папок с древовидной структурой
void displayContents(const fs::path& path, const std::string& prefix = "") {
    try {
        // Проверяем, существует ли путь
        if (!fs::exists(path)) {
            std::cout << prefix << "Ошибка: Указанный путь не существует!" << std::endl;
            return;
        }

        // Проверяем, является ли путь директорией
        if (!fs::is_directory(path)) {
            std::cout << prefix << "Ошибка: Указанный путь не является папкой!" << std::endl;
            return;
        }

        // Создаем вектор для хранения элементов
        std::vector<fs::directory_entry> items;

        // Собираем все элементы в текущей папке
        for (const auto& entry : fs::directory_iterator(path)) {
            items.push_back(entry);
        }

        // Сортируем элементы: сначала папки, потом файлы
        std::sort(items.begin(), items.end(),
            [](const fs::directory_entry& a, const fs::directory_entry& b) {
                bool a_is_dir = fs::is_directory(a);
                bool b_is_dir = fs::is_directory(b);

                if (a_is_dir && !b_is_dir) return true;
                if (!a_is_dir && b_is_dir) return false;

                // Если оба одного типа, сортируем по имени
                return a.path().filename().string() < b.path().filename().string();
            });

        // Выводим содержимое текущей папки
        for (size_t i = 0; i < items.size(); ++i) {
            const auto& item = items[i];
            bool is_last = (i == items.size() - 1);

            // Получаем имя файла или папки
            std::string name = item.path().filename().string();

            // Определяем префикс для текущего элемента
            std::string current_prefix = prefix;
            if (is_last) {
                current_prefix += "+-- ";
            }
            else {
                current_prefix += "+-- ";
            }

            if (fs::is_directory(item)) {
                // Это папка
                std::cout << current_prefix << name << "/" << std::endl;

                // Определяем префикс для вложенных элементов
                std::string new_prefix = prefix;
                if (is_last) {
                    new_prefix += "    ";
                }
                else {
                    new_prefix += "|   ";
                }

                // Рекурсивно обходим вложенную папку
                displayContents(item.path(), new_prefix);
            }
            else {
                // Это файл - получаем размер
                try {
                    uintmax_t size = fs::file_size(item);
                    std::cout << current_prefix << name << " (" << size << " bytes)" << std::endl;
                }
                catch (const fs::filesystem_error&) {
                    std::cout << current_prefix << name << " (unknown size)" << std::endl;
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cout << prefix << "Ошибка файловой системы: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << prefix << "Ошибка: " << e.what() << std::endl;
    }
}

int main() {
    // Для поддержки русского языка в Windows

    SetConsoleOutputCP(65001); // UTF-8
    SetConsoleCP(65001);


    std::string path;
    std::cout << "Enter path: ";
    std::getline(std::cin, path);

    // Удаляем возможные кавычки
    if (path.size() >= 2 && path.front() == '"' && path.back() == '"') {
        path = path.substr(1, path.size() - 2);
    }

    // Заменяем обратные слеши на прямые для кросс-платформенности
    for (char& c : path) {
        if (c == '\\') c = '/';
    }

    std::cout << "\nContents of " << path << ":\n";
    std::cout << "================================\n";

    // Показываем корневую папку
    size_t last_slash = path.find_last_of('/');
    std::string root_name = (last_slash != std::string::npos) ?
        path.substr(last_slash + 1) : path;
    if (root_name.empty()) root_name = path;
    std::cout << root_name << "/" << std::endl;

    // Начинаем обход с префиксом для вложенных элементов корня
    displayContents(path, "");

    return 0;
}


