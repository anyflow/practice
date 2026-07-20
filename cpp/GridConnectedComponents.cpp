/*
INTERVIEW QUESTION — Number of Islands (20분)

`'1'`은 땅, `'0'`은 물인 직사각형 격자가 주어진다.
상하좌우로 연결된 땅을 하나의 연결 요소로 볼 때 연결 요소의 개수를 반환하라.

    std::size_t countIslands(std::vector<std::string> grid);

동작 계약

- 대각선으로 맞닿은 땅은 연결되지 않는다.
- 빈 격자는 0을 반환한다.
- 입력은 각 행의 길이가 같은 직사각형이다.
- 방문한 땅은 입력 복사본에서 물로 바꿔 중복 방문을 막는다.
- 목표 시간복잡도는 O(rows * columns)다.
*/

// clang++ -std=c++20 -Wall -Wextra -Wpedantic \
//     -fsanitize=address,undefined -fno-omit-frame-pointer \
//     GridConnectedComponents.cpp -o grid-components && ./grid-components

#include <cassert>
#include <cstddef>
#include <queue>
#include <string>
#include <utility>
#include <vector>

void visitIsland(std::vector<std::string>& grid, int startRow, int startColumn) {
    const int rowCount = static_cast<int>(grid.size());
    const int columnCount = static_cast<int>(grid.front().size());
    constexpr int directions[][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1},
    };

    std::queue<std::pair<int, int>> pending;

    // queue에 넣기 직전 방문 처리해 같은 땅이 중복으로 들어가지 않게 한다.
    grid[startRow][startColumn] = '0';
    pending.emplace(startRow, startColumn);

    while (!pending.empty()) {
        const auto [row, column] = pending.front();
        pending.pop();

        for (const auto& direction : directions) {
            const int nextRow = row + direction[0];
            const int nextColumn = column + direction[1];

            if (nextRow < 0 || nextRow >= rowCount ||
                nextColumn < 0 || nextColumn >= columnCount ||
                grid[nextRow][nextColumn] != '1') {
                continue;
            }

            grid[nextRow][nextColumn] = '0';
            pending.emplace(nextRow, nextColumn);
        }
    }
}

std::size_t countIslands(std::vector<std::string> grid) {
    if (grid.empty() || grid.front().empty()) {
        return 0;
    }

    std::size_t islandCount = 0;

    for (int row = 0; row < static_cast<int>(grid.size()); ++row) {
        for (int column = 0;
             column < static_cast<int>(grid[row].size());
             ++column) {
            if (grid[row][column] != '1') {
                continue;
            }

            // 아직 방문하지 않은 땅 하나가 새 연결 요소의 시작점이다.
            ++islandCount;
            visitIsland(grid, row, column);
        }
    }

    return islandCount;
}

int main() {
    assert(countIslands({
        "11000",
        "11000",
        "00100",
        "00011",
    }) == 3);

    assert(countIslands({
        "000",
        "000",
    }) == 0);

    assert(countIslands({
        "111",
        "111",
    }) == 1);

    assert(countIslands({
        "101",
        "000",
        "101",
    }) == 4);

    assert(countIslands({}) == 0);
    assert(countIslands({""}) == 0);
}
