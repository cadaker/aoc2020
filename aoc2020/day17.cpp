#include "range_helpers.hpp"
#include <iostream>
#include <tuple>
#include <unordered_map>

struct pos_t {
    int x{};
    int y{};
    int z{};

    inline std::strong_ordering operator<=>(pos_t const& pos) const = default;
};

namespace std {
    template<>
    struct hash<pos_t> {
        size_t operator()(pos_t const& p) const {
            std::hash<int> h;
            auto combine = [](size_t h0, size_t h1) { return (h0 << 1) ^ h1; };
            return combine(combine(h(p.x), h(p.y)), h(p.z));
        }
    };
}

using world_t = std::unordered_map<pos_t, bool>;

struct world_limits_t {
    int xmin = 0;
    int xmax = 0;
    int ymin = 0;
    int ymax = 0;
    int zmin = 0;
    int zmax = 0;

    void expand(pos_t p) noexcept {
        xmin = std::min(xmin, p.x);
        xmax = std::max(xmax, p.x);
        ymin = std::min(ymin, p.y);
        ymax = std::max(ymax, p.y);
        zmin = std::min(zmin, p.z);
        zmax = std::max(zmax, p.z);
    }
};

std::pair<world_t, world_limits_t> parse(std::istream& is) {
    world_limits_t limits{0, 0, 0, 0, 0, 0};
    world_t world;
    std::string line;
    int y = 0;
    while (std::getline(is, line)) {
        for (int x = 0; x < static_cast<int>(line.length()); ++x) {
            if (line[x] == '#') {
                world[{x, y, 0}] = true;
                limits.expand({x, y, 0});
            }
        }
        ++y;
    }
    return {world, limits};
}

int active_neighbours(world_t const& world, pos_t pos) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                auto it = world.find({pos.x + dx, pos.y + dy, pos.z + dz});
                if ((dx != 0 || dy != 0 || dz != 0) && it != world.end() && it->second) {
                    ++count;
                }
            }
        }
    }
    return count;
}

void evolve(world_t& world, world_limits_t& limits) {
    world_t new_world;
    world_limits_t new_limits = limits;
    for (int x = limits.xmin-1; x <= limits.xmax+1; ++x) {
        for (int y = limits.ymin-1; y <= limits.ymax+1; ++y) {
            for (int z = limits.zmin-1; z <= limits.zmax+1; ++z) {
                int const n = active_neighbours(world, {x, y, z});
                if ((!world[{x, y, z}] && n == 3) ||
                    (world[{x, y, z}] && (n == 2 || n == 3))) {
                    new_world[{x, y, z}] = true;
                    new_limits.expand({x, y, z});
                }
            }
        }
    }
    world = std::move(new_world);
    limits = new_limits;
}

size_t count_active(world_t const& world) {
    return world | std::views::transform([](auto& p) { return static_cast<unsigned long>(p.second); }) | accumulate(0UL);
}

void run() {
    auto [world, limits] = parse(std::cin);

    for (int iter = 0; iter < 6; ++iter) {
        evolve(world, limits);
    }
    std::cout << count_active(world) << std::endl;
}
