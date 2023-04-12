#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
template <class T>
struct Less {
    bool operator() (const std::pair<T, int>& e1, const std::pair<T, int>& e2) const {
        return e1.second < e2.second;
    }
};
template <class T>
class top_k_solution {
public:
    static std::vector<T> top_k(std::vector<T>& elements, int k) {
        std::unordered_map<T, int>stat_map;
        for (auto &e : elements) {
            auto it = stat_map.find(e);
            if (it == stat_map.end()) {
                stat_map.emplace(e, 1);
            }
            else {
                ++(it->second);
            }
        }
        std::priority_queue<std::pair<T, int>, std::vector<std::pair<T, int>>, Less<T>>pq;
        for (const auto &m : stat_map) {
            pq.push(m);    
        }
        if (k > pq.size()) {
            k = pq.size();
        }
        std::vector<T>tmp;
        for (int i = 0;i < k;i++) {
            tmp.emplace_back(pq.top().first);
            pq.pop();
        }
        return tmp;
    }
};