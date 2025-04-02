//
// Created by jh on 2025/4/2.
//

#pragma once

#include <unordered_map>
#include <mutex>
#include <optional>
#include <functional>

template<typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(size_t capacity): _capacity(capacity) {
        if (capacity <= 0) {
            throw std::invalid_argument("LRUCache capacity must be greater than 0!");
        }
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(_lock);
        return _map.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(_lock);
        _map.clear();
        if (_head) {
            _head->next = nullptr;
            _head = nullptr;
        }
        if (_tail) {
            _tail->prev = nullptr;
            _tail = nullptr;
        }
    }

    void put(const Key& key, Value value) {
        std::lock_guard<std::mutex> lock(_lock);

        // 如果key已存在，更新对应的value并将其移到头部
        if (auto it = _map.find(key); it != _map.end()) {
            moveToHead(it->second.get());
            it->second->value = value;
        }
        // 如果超过缓存大小，移除最近最少使用的项
        if (_map.size() >= _capacity) {
            evict();
        }
        // 创建节点添加到头部
        auto node = std::make_unique<Node>(key, std::move(value));
        addToHead(node.get());
        _map[key] = std::move(node);
    }

    std::optional<Value> get(const Key& key) {
        std::lock_guard<std::mutex> lock(_lock);
        if (auto it = _map.find(key); it != _map.end()) {
            moveToHead(it->second.get());
            return it->second->value;
        }
        return std::nullopt;
    }

    void foreach(const std::function<void(const Key& key, const Value& value)> func) {
        std::lock_guard<std::mutex> lock(_lock);
        for (Node* current = _head; current != nullptr; current = current->next) {
            func(current->key, current->value);
        }
    }
private:
    struct Node {
        Key key;
        Value value;
        Node* prev = nullptr;
        Node* next = nullptr;

        Node(Key k, Value v): key(std::move(k)), value(std::move(v)) {}
    };

    void addToHead(Node* node) {
        if (!node) return;

        if (_head) {
            node->next = _head;
            _head->prev = node;
            _head = node;
        } else {
            _head = node;
            _tail = _head;
        }
    }

    void removeNode(Node* node) {
        if (!node) return;

        if (node->prev) {
            node->prev->next = node->next;
        }
        if (node->next) {
            node->next->prev = node->prev;
        }
        if (node == _head) {
            _head = node->next;
        }
        if (node == _tail) {
            _tail = node->prev;
        }
    }

    void moveToHead(Node* node) {
        if (node == _head) return;
        removeNode(node);
        addToHead(node);
    }

    void evict() {
        if (_tail) {
            _map.erase(_tail->key);
            removeNode(_tail);
        }
    }

    const size_t _capacity;
    Node* _head = nullptr;
    Node* _tail = nullptr;
    std::unordered_map<Key, std::unique_ptr<Node>> _map;
    mutable std::mutex _lock;
};