#include "Util/GameObject.hpp"
#include "Util/Transform.hpp"
#include "Util/TransformUtils.hpp"

namespace Util {

void GameObject::Draw(Util::Transform &transform) {
    if (!m_Visible || m_Drawable == nullptr) {
        return;
    }

    auto data = Util::ConvertToUniformBufferData(
        transform, m_Drawable->GetSize(), m_ZIndex);
    data.m_Model = glm::translate(
        data.m_Model, glm::vec3{m_Pivot / m_Drawable->GetSize(), 0} * -1.0F);

    m_Drawable->Draw(data);
}

void GameObject::Draw() {
    Draw(m_Transform);
}

} // namespace Util
