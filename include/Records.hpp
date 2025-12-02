#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <functional>


struct Records {
    std::vector<int> last; // топ 5
    std::string primaryPath{"data/records.txt"};
    std::string secondaryPath{"../data/records.txt"};
    std::string currentPath;

    bool load();

    // Сохранить новый результат и переписать файл
    void add(int score);

private:
    // Вспомогательная загрузка по обоим путям
    bool loadFrom(const std::string& path);

    // Отсортировать по убыванию и оставить пять лучших
    void keepTop();
};
