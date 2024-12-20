#include "OcTree.h"
#include "../objects/base/GameObject.h"
#include "../objects/base/GameObject.h"
#include "../objects/debugTools/Line.h"
#include <vector>

template <typename T>
void OcTree<T>::recalculate()
{
    throw "Cannot recalculate OcTree of unknown type";
}

template <>
void OcTree<GameObject*>::recalculate()
{
    std::vector<GameObject*> all_objects;
    query<GameObject*>(all_objects);

    for (auto& object : all_objects)
    {
        pop(object);
    }
    unsubdivide();
    for (auto& object : all_objects)
    {
        insert(object);
    }
}

void OcTreeBase::draw_debug(Line* line, bool draw_bounds)
{
    if (draw_bounds)
    {
        boundary.draw_debug(line);
    }
    if (is_leaf())
        return;
    auto center = boundary.center;
    auto extent = boundary.extent;
    auto extentDivide = extent / 2.0f;
    line->set_positions(center + glm::vec3(-extent.x, 0, 0), center + glm::vec3(extent.x, 0, 0));
    line->draw();
    line->set_positions(center + glm::vec3(0, -extent.y, 0), center + glm::vec3(0, extent.y, 0));
    line->draw();
    line->set_positions(center + glm::vec3(0, 0, -extent.z), center + glm::vec3(0, 0, extent.z));
    line->draw();
    line->set_positions(center + glm::vec3(extent.x, -extent.y, 0), center + glm::vec3(extent.x, extent.y, 0));
    line->draw();
    line->set_positions(center + glm::vec3(extent.x, 0, -extent.z), center + glm::vec3(extent.x, 0, extent.z));
    line->draw();
    line->set_positions(center + glm::vec3(-extent.x, -extent.y, 0), center + glm::vec3(-extent.x, extent.y, 0));
    line->draw();
    line->set_positions(center + glm::vec3(-extent.x, 0, -extent.z), center + glm::vec3(-extent.x, 0, extent.z));
    line->draw();
    line->set_positions(center + glm::vec3(-extent.x, extent.y, 0), center + glm::vec3(extent.x, extent.y, 0));
    line->draw();
    line->set_positions(center + glm::vec3(0, extent.y, -extent.z), center + glm::vec3(0, extent.y, extent.z));
    line->draw();
    line->set_positions(center + glm::vec3(-extent.x, -extent.y, 0), center + glm::vec3(extent.x, -extent.y, 0));
    line->draw();
    line->set_positions(center + glm::vec3(0, -extent.y, -extent.z), center + glm::vec3(0, -extent.y, extent.z));
    line->draw();
    line->set_positions(center + glm::vec3(-extent.x, 0, extent.z), center + glm::vec3(extent.x, 0, extent.z));
    line->draw();
    line->set_positions(center + glm::vec3(0, -extent.y, extent.z), center + glm::vec3(0, extent.y, extent.z));
    line->draw();
    line->set_positions(center + glm::vec3(-extent.x, 0, -extent.z), center + glm::vec3(extent.x, 0, -extent.z));
    line->draw();
    line->set_positions(center + glm::vec3(0, -extent.y, -extent.z), center + glm::vec3(0, extent.y, -extent.z));
    line->draw();
}