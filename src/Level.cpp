#include "Level.hpp"


bool Level::loadFromFile(const std::string& file) {
    // читаем строки карты и проверяем размеры
    std::ifstream in(file);
    if (!in) return false;

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        lines.push_back(line);
    }
    if (lines.size() != GridSize) return false;
    for (auto& l : lines) {
        if (l.size() != GridSize) return false;
    }

    pads.clear();
    path.waypoints.clear();
    std::optional<sf::Vector2i> spawnCell;
    std::optional<sf::Vector2i> baseCell;

    // разбираем символы сетки и ищем спавн и базу
    for (int y = 0; y < GridSize; ++y) {
        grid[y] = lines[y];
        for (int x = 0; x < GridSize; ++x) {
            char c = grid[y][x];
            if (c == '#') pads.push_back({cellCenter(x, y)});
            if (c == '@' && !spawnCell) spawnCell = sf::Vector2i{x, y};
            if (c == '^') baseCell = sf::Vector2i{x, y};
        }
    }
    if (!spawnCell || !baseCell) return false;

    auto isWalkable = [&](int x, int y) {
        if (x < 0 || y < 0 || x >= GridSize || y >= GridSize) return false;
        char c = grid[y][x];
        return c == '@' || c == '^';
    };

    auto indexOf = [](int x, int y) { return y * GridSize + x; };
    auto fromIndex = [](int idx) {
        return sf::Vector2i{idx % GridSize, idx / GridSize};
    };

    // bfs по сетке чтобы восстановить путь
    std::queue<sf::Vector2i> q;
    std::vector<int> parent(GridSize * GridSize, -1);
    std::vector<bool> visited(GridSize * GridSize, false);

    q.push(*spawnCell);
    visited[indexOf(spawnCell->x, spawnCell->y)] = true;

    bool reached = false;
    while (!q.empty()) {
        sf::Vector2i cur = q.front();
        q.pop();
        if (cur == *baseCell) { reached = true; break; }
        const sf::Vector2i directions[4] = {{1,0},{-1,0},{0,1},{0,-1}};
        for (auto d : directions) {
            int nx = cur.x + d.x;
            int ny = cur.y + d.y;
            if (!isWalkable(nx, ny)) continue;
            int nIndex = indexOf(nx, ny);
            if (visited[nIndex]) continue;
            visited[nIndex] = true;
            parent[nIndex] = indexOf(cur.x, cur.y);
            q.push({nx, ny});
        }
    }
    if (!reached) return false;

    std::vector<sf::Vector2i> cells;
    sf::Vector2i current = *baseCell;
    cells.push_back(current);
    while (current != *spawnCell) {
        int pIdx = parent[indexOf(current.x, current.y)];
        if (pIdx < 0) return false;
        current = fromIndex(pIdx);
        cells.push_back(current);
    }
    std::reverse(cells.begin(), cells.end());

    path.waypoints.clear();
    for (auto cell : cells) path.waypoints.push_back(cellCenter(cell.x, cell.y));

    spawnPos = cellCenter(spawnCell->x, spawnCell->y);
    basePos = cellCenter(baseCell->x, baseCell->y);
    hasGrid = true;
    return true;
}


sf::Vector2f Level::cellCenter(int x, int y) const {
    return {origin.x + (x + 0.5f) * tileSize, origin.y + (y + 0.5f) * tileSize};
}


bool Level::isBuildAllowed(const sf::Vector2f& p) const {
    // проверяем, близко ли к разрешённой площадке
    for (auto& pad : pads) {
        if (std::hypot(pad.pos.x - p.x, pad.pos.y - p.y) < 18) return true;
    }
    return false;
}


int Level::nearestPadIndex(const sf::Vector2f& p) const {
    // поиск индекса ближайшей площадки для меню
    int idx=-1; float best=1e9f;
    for (size_t i=0;i<pads.size();++i) {
        float d=std::hypot(pads[i].pos.x-p.x,pads[i].pos.y-p.y);
        if (d<best) {best=d; idx=(int)i;}
    }
    return idx;
}
