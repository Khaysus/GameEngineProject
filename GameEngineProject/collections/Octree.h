#pragma once

#include <vector>
#include "../colliders/AABB.h"
#include "Node.h"
#include "../culling/Frustum.h"

class OcTreeBase
{
private:
    AABB boundary = AABB();
    bool leaf;

public:
    void set_bounds(const glm::vec3& center, const glm::vec3& extent)
    {
        boundary.center = center;
        boundary.extent = extent;
        boundary.recalculate();
    };

    AABB get_bounds() { return boundary; }
    virtual void draw_debug(Line* line, bool draw_bounds = true);
    bool is_leaf() const { return leaf; }
    void set_leaf(bool vleaf) { leaf = vleaf; }
};

template <typename T>
class OcTree : public OcTreeBase
{
private:
    OcTree* northWestUpper;
    OcTree* northEastUpper;
    OcTree* southWestUpper;
    OcTree* southEastUpper;
    OcTree* northWestLower;
    OcTree* northEastLower;
    OcTree* southWestLower;
    OcTree* southEastLower;
    OcTree* parent;

    bool recalculated = false;

    Node<T>* node = nullptr;

    void subdivide()
    {
        glm::vec3 center = get_bounds().center;
        glm::vec3 extent = get_bounds().extent;
        glm::vec3 extentDivide = extent / 2.0f;

        glm::vec3 nwu_center = center + glm::vec3(-extent.x / 2, extent.y / 2, extent.z / 2);
        glm::vec3 neu_center = center + glm::vec3(extent.x / 2, extent.y / 2, extent.z / 2);
        glm::vec3 swu_center = center + glm::vec3(-extent.x / 2, extent.y / 2, -extent.z / 2);
        glm::vec3 seu_center = center + glm::vec3(extent.x / 2, extent.y / 2, -extent.z / 2);
        glm::vec3 nwl_center = center + glm::vec3(-extent.x / 2, -extent.y / 2, extent.z / 2);
        glm::vec3 nel_center = center + glm::vec3(extent.x / 2, -extent.y / 2, extent.z / 2);
        glm::vec3 swl_center = center + glm::vec3(-extent.x / 2, -extent.y / 2, -extent.z / 2);
        glm::vec3 sel_center = center + glm::vec3(extent.x / 2, -extent.y / 2, -extent.z / 2);

        northWestUpper = new OcTree<T>();
        northWestUpper->set_bounds(nwu_center, extentDivide);
        northEastUpper = new OcTree<T>();
        northEastUpper->set_bounds(neu_center, extentDivide);
        southWestUpper = new OcTree<T>();
        southWestUpper->set_bounds(swu_center, extentDivide);
        southEastUpper = new OcTree<T>();
        southEastUpper->set_bounds(seu_center, extentDivide);
        northWestLower = new OcTree<T>();
        northWestLower->set_bounds(nwl_center, extentDivide);
        northEastLower = new OcTree<T>();
        northEastLower->set_bounds(nel_center, extentDivide);
        southWestLower = new OcTree<T>();
        southWestLower->set_bounds(swl_center, extentDivide);
        southEastLower = new OcTree<T>();
        southEastLower->set_bounds(sel_center, extentDivide);
        northWestUpper->parent = this;
        northEastUpper->parent = this;
        southWestUpper->parent = this;
        southEastUpper->parent = this;
        northWestLower->parent = this;
        northEastLower->parent = this;
        southWestLower->parent = this;
        southEastLower->parent = this;
        set_leaf(false);

        if (northWestUpper->insert(node->data))
        {
            delete node;
            node = nullptr;
            return;
        }
        if (northEastUpper->insert(node->data))
        {
            delete node;
            node = nullptr;
            return;
        }
        if (southWestUpper->insert(node->data))
        {
            delete node;
            node = nullptr;
            return;
        }
        if (southEastUpper->insert(node->data))
        {
            delete node;
            node = nullptr;
            return;
        }
        if (northWestLower->insert(node->data))
        {
            delete node;
            node = nullptr;
            return;
        }
        if (northEastLower->insert(node->data))
        {
            delete node;
            node = nullptr;
            return;
        }
        if (southWestLower->insert(node->data))
        {
            delete node;
            node = nullptr;
            return;
        }
        southEastLower->insert(node->data);
        delete node;
        node = nullptr;
    }

    bool unsubdivide()
    {
        if (!is_leaf())
        {
            northWestUpper->unsubdivide();
            northEastUpper->unsubdivide();
            southWestUpper->unsubdivide();
            southEastUpper->unsubdivide();
            northWestLower->unsubdivide();
            northEastLower->unsubdivide();
            southWestLower->unsubdivide();
            southEastLower->unsubdivide();
            if (northWestUpper->node == nullptr && northEastUpper->node == nullptr && southWestUpper->node == nullptr && southEastUpper->node == nullptr &&
                northWestLower->node == nullptr && northEastLower->node == nullptr && southWestLower->node == nullptr && southEastLower->node == nullptr)
            {
                delete northWestUpper;
                delete northEastUpper;
                delete southWestUpper;
                delete southEastUpper;
                delete northWestLower;
                delete northEastLower;
                delete southWestLower;
                delete southEastLower;
                northWestUpper = nullptr;
                northEastUpper = nullptr;
                southWestUpper = nullptr;
                southEastUpper = nullptr;
                northWestLower = nullptr;
                northEastLower = nullptr;
                southWestLower = nullptr;
                southEastLower = nullptr;
                set_leaf(true);
                return true;
            }
        }
        return false;
    }

public:
    OcTree() : northWestUpper(nullptr), northEastUpper(nullptr), southWestUpper(nullptr), southEastUpper(nullptr),
        northWestLower(nullptr), northEastLower(nullptr), southWestLower(nullptr), southEastLower(nullptr)
    {
        set_leaf(true);
    };
    ~OcTree()
    {
        delete northWestUpper;
        delete northEastUpper;
        delete southWestUpper;
        delete southEastUpper;
        delete northWestLower;
        delete northEastLower;
        delete southWestLower;
        delete southEastLower;
    };

    bool insert(T point, int max_depth = 20)
    {
        if (!get_bounds().contains(point))
            return false;

        if (node == nullptr && is_leaf())
        {
            node = new Node<T>();
            node->data = point;
            return true;
        }

        if (max_depth == 0)
            return false;

        if (is_leaf())
            subdivide();

        if (northWestUpper->insert(point, max_depth - 1))
            return true;
        if (northEastUpper->insert(point, max_depth - 1))
            return true;
        if (southWestUpper->insert(point, max_depth - 1))
            return true;
        if (southEastUpper->insert(point, max_depth - 1))
            return true;
        if (northWestLower->insert(point, max_depth - 1))
            return true;
        if (northEastLower->insert(point, max_depth - 1))
            return true;
        if (southWestLower->insert(point, max_depth - 1))
            return true;
        if (southEastLower->insert(point, max_depth - 1))
            return true;
        return false;
    };
    T pop(T point)
    {
        T result = nullptr;
        if (node != nullptr && node->data == point && is_leaf())
        {
            result = node->data;
            delete node;
            node = nullptr;
            return result;
        }
        if (northWestUpper == nullptr)
            return result;
        result = northWestUpper->pop(point);
        if (result != nullptr)
            return result;
        result = northEastUpper->pop(point);
        if (result != nullptr)
            return result;
        result = southWestUpper->pop(point);
        if (result != nullptr)
            return result;
        result = southEastUpper->pop(point);
        if (result != nullptr)
            return result;
        result = northWestLower->pop(point);
        if (result != nullptr)
            return result;
        result = northEastLower->pop(point);
        if (result != nullptr)
            return result;
        result = southWestLower->pop(point);
        if (result != nullptr)
            return result;
        result = southEastLower->pop(point);
        return result;
    }
    template <typename F>
    std::tuple<unsigned, unsigned> query_range(AABB range, std::vector<F>& found, Frustum* frustum = nullptr)
    {
        auto total = 0;
        auto found_count = 0;
        auto bounds = get_bounds();
        if (frustum == nullptr)
        {
            if (!bounds.contains(range))
                return std::make_tuple(total, found_count);
        }
        else
        {
            if (!bounds.contains(range) || !bounds.is_on_frustum(frustum))
                return std::make_tuple(total, found_count);
        }

        F data = nullptr;
        if (node != nullptr)
        {
            data = dynamic_cast<F>(node->data);
            total++;
        }

        if (data != nullptr && range.contains(data))
        {
            found_count++;
            found.push_back(data);
        }

        if (is_leaf())
            return std::make_tuple(total, found_count);

        auto tmp_tuple = northWestUpper->query_range(range, found, frustum);
        total += std::get<0>(tmp_tuple);
        found_count += std::get<1>(tmp_tuple);
        tmp_tuple = northEastUpper->query_range(range, found, frustum);
        total += std::get<0>(tmp_tuple);
        found_count += std::get<1>(tmp_tuple);
        tmp_tuple = southWestUpper->query_range(range, found, frustum);
        total += std::get<0>(tmp_tuple);
        found_count += std::get<1>(tmp_tuple);
        tmp_tuple = southEastUpper->query_range(range, found, frustum);
        total += std::get<0>(tmp_tuple);
        found_count += std::get<1>(tmp_tuple);
        tmp_tuple = northWestLower->query_range(range, found, frustum);
        total += std::get<0>(tmp_tuple);
        found_count += std::get<1>(tmp_tuple);
        tmp_tuple = northEastLower->query_range(range, found, frustum);
        total += std::get<0>(tmp_tuple);
        found_count += std::get<1>(tmp_tuple);
        tmp_tuple = southWestLower->query_range(range, found, frustum);
        total += std::get<0>(tmp_tuple);
        found_count += std::get<1>(tmp_tuple);
        tmp_tuple = southEastLower->query_range(range, found, frustum);
        total += std::get<0>(tmp_tuple);
        found_count += std::get<1>(tmp_tuple);
        return std::make_tuple(total, found_count);
    };

    template <typename F>
    void query_range(AABB range, std::vector<F>& found, std::function<bool(const F)> filter)
    {
        if (!get_bounds().contains(range))
            return;

        F data = nullptr;
        if (node != nullptr)
            data = dynamic_cast<F>(node->data);

        if (data != nullptr && range.contains(data) && filter(data))
            found.push_back(data);

        if (is_leaf())
            return;

        northWestUpper->query_range(range, found, filter);
        northEastUpper->query_range(range, found, filter);
        southWestUpper->query_range(range, found, filter);
        southEastUpper->query_range(range, found, filter);
        northWestLower->query_range(range, found, filter);
        northEastLower->query_range(range, found, filter);
        southWestLower->query_range(range, found, filter);
        southEastLower->query_range(range, found, filter);
    };

    template <typename F>
    void query(std::vector<F>& found)
    {
        F data = nullptr;
        if (node != nullptr)
            data = dynamic_cast<F>(node->data);

        if (data != nullptr)
            found.push_back(data);

        if (is_leaf())
            return;

        northWestUpper->query(found);
        northEastUpper->query(found);
        southWestUpper->query(found);
        southEastUpper->query(found);
        northWestLower->query(found);
        northEastLower->query(found);
        southWestLower->query(found);
        southEastLower->query(found);
    }

    template <typename F>
    void query(std::vector<F>& found, std::function<bool(const F)> filter)
    {
        F data = nullptr;
        if (node != nullptr)
            data = dynamic_cast<F>(node->data);

        if (data != nullptr && filter(data))
            found.push_back(data);

        if (is_leaf())
            return;

        northWestUpper->query(found, filter);
        northEastUpper->query(found, filter);
        southWestUpper->query(found, filter);
        southEastUpper->query(found, filter);
        northWestLower->query(found, filter);
        northEastLower->query(found, filter);
    }

    template <typename F>
    std::tuple<unsigned, unsigned> query(std::vector<F>& found, Frustum* frustum, std::function<glm::vec3(const F)> get_position)
    {
        auto total = 0;
        auto found_count = 0;
        F data = nullptr;
        if (node != nullptr)
        {
            data = dynamic_cast<F>(node->data);
            total++;
        }
        if (data != nullptr && frustum->contains(get_position(data)))
        {
            found.push_back(data);
            found_count++;
        }

        if (is_leaf())
            return std::make_tuple(total, found_count);

        auto tmp = northWestUpper->query(found, frustum, get_position);
        total += std::get<0>(tmp);
        found_count += std::get<1>(tmp);
        tmp = northEastUpper->query(found, frustum, get_position);
        total += std::get<0>(tmp);
        found_count += std::get<1>(tmp);
        tmp = southWestUpper->query(found, frustum, get_position);
        total += std::get<0>(tmp);
        found_count += std::get<1>(tmp);
        tmp = southEastUpper->query(found, frustum, get_position);
        total += std::get<0>(tmp);
        found_count += std::get<1>(tmp);
        tmp = northWestLower->query(found, frustum, get_position);
        total += std::get<0>(tmp);
        found_count += std::get<1>(tmp);
        tmp = northEastLower->query(found, frustum, get_position);
        total += std::get<0>(tmp);
        found_count += std::get<1>(tmp);
        tmp = southWestLower->query(found, frustum, get_position);
        total += std::get<0>(tmp);
        found_count += std::get<1>(tmp);
        tmp = southEastLower->query(found, frustum, get_position);
        total += std::get<0>(tmp);
        found_count += std::get<1>(tmp);
        return std::make_tuple(total, found_count);
    }

    void recalculate();
    void draw_debug(Line* line, bool draw_bounds = true) override
    {
        OcTreeBase::draw_debug(line, draw_bounds);
        if (is_leaf())
            return;
        northWestUpper->draw_debug(line, false);
        northEastUpper->draw_debug(line, false);
        southWestUpper->draw_debug(line, false);
        southEastUpper->draw_debug(line, false);
        northWestLower->draw_debug(line, false);
        northEastLower->draw_debug(line, false);
        southWestLower->draw_debug(line, false);
        southEastLower->draw_debug(line, false);
    }

    T get_node(std::function<bool(T)> predicate)
    {
        if (node != nullptr && predicate(node->data))
            return node->data;
        if (is_leaf())
            return nullptr;
        auto result = northWestUpper->get_node(predicate);
        if (result != nullptr)
            return result;
        result = northEastUpper->get_node(predicate);
        if (result != nullptr)
            return result;
        result = southWestUpper->get_node(predicate);
        if (result != nullptr)
            return result;
        result = southEastUpper->get_node(predicate);
        if (result != nullptr)
            return result;
        result = northWestLower->get_node(predicate);
        if (result != nullptr)
            return result;
        result = northEastLower->get_node(predicate);
        if (result != nullptr)
            return result;
        result = southWestLower->get_node(predicate);
        if (result != nullptr)
            return result;
        return southEastLower->get_node(predicate);
    }

    void clear()
    {
        if (node != nullptr)
        {
            delete node;
            node = nullptr;
        }
        if (northWestUpper != nullptr)
        {
            delete northWestUpper;
            northWestUpper = nullptr;
        }
        if (northEastUpper != nullptr)
        {
            delete northEastUpper;
            northEastUpper = nullptr;
        }
        if (southWestUpper != nullptr)
        {
            delete southWestUpper;
            southWestUpper = nullptr;
        }
        if (southEastUpper != nullptr)
        {
            delete southEastUpper;
            southEastUpper = nullptr;
        }
        if (northWestLower != nullptr)
        {
            delete northWestLower;
            northWestLower = nullptr;
        }
        if (northEastLower != nullptr)
        {
            delete northEastLower;
            northEastLower = nullptr;
        }
        if (southWestLower != nullptr)
        {
            delete southWestLower;
            southWestLower = nullptr;
        }
        if (southEastLower != nullptr)
        {
            delete southEastLower;
            southEastLower = nullptr;
        }
        set_leaf(true);
    }
};