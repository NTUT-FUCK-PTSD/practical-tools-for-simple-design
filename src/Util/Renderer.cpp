#include "Util/Renderer.hpp"

#include <queue>

#include "Util/Logger.hpp"
#include "config.hpp"

namespace Util {
Renderer::Renderer(const std::vector<std::shared_ptr<GameObject>> &children)
    : m_Children(children) {}

void Renderer::AddChild(const std::shared_ptr<GameObject> &child) {
    m_Children.push_back(child);
}

void Renderer::RemoveChild(std::shared_ptr<GameObject> child) {
    m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), child),
                     m_Children.end());
}

void Renderer::AddChildren(
    const std::vector<std::shared_ptr<GameObject>> &children) {
    m_Children.reserve(m_Children.size() + children.size());
    m_Children.insert(m_Children.end(), children.begin(), children.end());
}

void Renderer::ClearChildren() {
    m_Children.clear();
}

void Renderer::Update(glm::vec2 translation) {
    struct StackInfo {
        std::shared_ptr<GameObject> m_GameObject;
        Transform m_ParentTransform;
    };

    std::vector<StackInfo> stack;
    stack.reserve(m_Children.size());

    for (const auto &child : m_Children) {
        stack.push_back(StackInfo{child, Transform{}});
    }

    auto compareFunction = [](const StackInfo &a, const StackInfo &b) {
        return a.m_GameObject->GetZIndex() > b.m_GameObject->GetZIndex();
    };
    std::priority_queue<StackInfo, std::vector<StackInfo>,
                        decltype(compareFunction)>
        renderQueue(compareFunction);

    while (!stack.empty()) {
        auto curr = stack.back();
        stack.pop_back();
        renderQueue.push(curr);

        for (const auto &child : curr.m_GameObject->GetChildren()) {
            stack.push_back(
                StackInfo{child, curr.m_GameObject->GetTransform()});
        }
    }
    // draw all in render queue by order
    while (!renderQueue.empty()) {
        auto curr = renderQueue.top();
        renderQueue.pop();

        if (translation.x == 0 && translation.y == 0) {
            curr.m_GameObject->Draw();
        } else {
            auto _translation = curr.m_GameObject->m_Transform.translation;
            _translation += translation;
            auto scaledSize = curr.m_GameObject->GetScaledSize() / 2.0f;
            // check if object is out of screen
            if (_translation.x - scaledSize.x >
                    (PTSD_Config::WINDOW_WIDTH >> 1) ||
                _translation.x + scaledSize.x <
                    -static_cast<int>(PTSD_Config::WINDOW_WIDTH >> 1) ||
                _translation.y - scaledSize.y >
                    (PTSD_Config::WINDOW_HEIGHT >> 1) ||
                _translation.y + scaledSize.y <
                    -static_cast<int>(PTSD_Config::WINDOW_HEIGHT >> 1)) {
                continue;
            } else {
                Util::Transform transform = {
                    _translation, curr.m_GameObject->m_Transform.rotation,
                    curr.m_GameObject->m_Transform.scale};
                curr.m_GameObject->Draw(transform);
            }
        }
    }
}
} // namespace Util
