#include "Records.hpp"


bool Records::load() {
    // пробуем два пути хранения рекордов
    if (loadFrom(primaryPath)) return true;
    if (loadFrom(secondaryPath)) return true;
    last.clear();
    currentPath = primaryPath;
    return false;
}


void Records::add(int score) {
    // добавляем новое значение и сохраняем в файл
    if (currentPath.empty()) currentPath = primaryPath;
    last.push_back(score);
    keepTop();
    std::ofstream out(currentPath, std::ios::trunc);
    if (!out && currentPath != primaryPath) {
        currentPath = primaryPath;
        out.open(currentPath, std::ios::trunc);
    }
    for (auto v : last) out << v << "\n";
}


bool Records::loadFrom(const std::string& path) {
    // загрузка значений из файла и сохранение активного пути
    std::ifstream in(path);
    if (!in) return false;
    std::vector<int> values;
    int v;
    while (in >> v) values.push_back(v);
    last = std::move(values);
    keepTop();
    currentPath = path;
    return true;
}


void Records::keepTop() {
    // сортировка по убыванию и оставление топ 5
    std::sort(last.begin(), last.end(), std::greater<int>());
    if (last.size() > 5) last.resize(5);
}
