#pragma once

// NOTE: This #define forces GLM to use radians (not degrees) for ALL of its
// angle arguments. The documentation may not always reflect this fact.
#define GLM_FORCE_RADIANS

// Primary GLM library
# include <../include/glm/glm.hpp>
// For glm::translate, glm::rotate, and glm::scale.
# include <../include/glm/gtc/matrix_transform.hpp>
// For glm::to_string.
# include <../include/glm/gtx/string_cast.hpp>
// For glm::value_ptr.
# include <../include/glm/gtc/type_ptr.hpp>

#include <QMatrix4x4>

/// 277 linear algebra namespace
namespace la
{
using namespace glm;

inline QMatrix4x4 to_qmat(const mat4 &m)
{
    return QMatrix4x4(value_ptr(transpose(m)));
}
}

/// Float approximate-equality comparison
template<typename T>
inline bool fequal(T a, T b, T epsilon = 0.0001){
    if (a == b) {
        // Shortcut
        return true;
    }

    const T diff = std::abs(a - b);
    if (a * b == 0) {
        // a or b or both are zero; relative error is not meaningful here
        return diff < (epsilon * epsilon);
    }

    return diff / (std::abs(a) + std::abs(b)) < epsilon;
}