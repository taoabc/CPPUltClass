/**
 * @file
 * @author huhaitao
 * @brief 最近访问缓存实现
 * @remark 默认使用unordered_map
 */
#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <functional>

template <typename K, typename T>
class CLRUCache
{
private:
    struct Node
    {
        K key;
        T data;
        Node* prev;
        Node* next;
    };
    typedef std::unordered_map<K, Node*> Map;
    typedef std::function<void(const T& data)> RecyleFunc;
    //typedef std::map<k, Node*> Map;
public:

    CLRUCache(size_t size)
    {
        m_entries = new Node[size];
        for (size_t i = 0; i < size; ++i)
        {
            m_freeEntries.push_back(m_entries + i);
        }
        m_head = new Node;
        m_tail = new Node;
        m_head->prev = nullptr;
        m_head->next = m_tail;
        m_tail->prev = m_head;
        m_tail->next = nullptr;
    }

    ~CLRUCache()
    {
        delete m_head;
        delete m_tail;
        if (m_recyleFunc)
        {
            std::for_each(m_map.begin(), m_map.end(),
                          [this](const Map::value_type& pair) { m_recyleFunc(pair.second->data); });
        }
        delete[] m_entries;
    }

    template <typename F>
    void SetRecyleFunc(const F& func)
    {
        m_recyleFunc = func;
    }

    bool Get(const K& key, T& data)
    {
        auto iter = m_map.find(key);
        if (iter != m_map.end())
        {
            Node* n = iter->second;
            DetachNode(n);
            AttachNode(n);
            data = n->data;
            return true;
        }
        else
        {
            return false;
        }
    }

    void Put(const K& key, const T& data)
    {
        auto iter = m_map.find(key);
        if (iter != m_map.end())
        {
            Node* n = iter->second;
            n->data = data;
            DetachNode(n);
            AttachNode(n);
        }
        else
        {
            Node* n = nullptr;
            if (m_freeEntries.empty())
            {
                n = m_tail->prev;
                if (m_recyleFunc)
                {
                    m_recyleFunc(n->data);
                }
                m_map.erase(n->key);
                DetachNode(n);
            }
            else
            {
                n = m_freeEntries.back();
                m_freeEntries.pop_back();
            }
            n->key = key;
            n->data = data;
            AttachNode(n);
            m_map[key] = n;
        }
    }

private:

    void AttachNode(Node* n)
    {
        n->prev = m_head;
        n->next = m_head->next;
        n->next->prev = n;
        m_head->next = n;
    }

    void DetachNode(Node* n)
    {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

    Map m_map;
    std::vector<Node*> m_freeEntries;
    RecyleFunc m_recyleFunc;
    Node* m_head;
    Node* m_tail;
    Node* m_entries;
};