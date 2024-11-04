#pragma once

#include <glm/mat4x4.hpp>

struct TinyMathLibrary
{
    glm::mat4 Translate(float tx, float ty, float tz)
    {
        return glm::mat4(1, 0, 0, tx,
                0, 1, 0, ty,
                0, 0, 1, tz,
                0, 0, 0, 1);
    }

    glm::mat4 Scale(float sx, float sy, float sz)
    {
        return glm::transpose(
            glm::mat4(sx, 0, 0, 0,
                0, sy, 0, 0,
                0, 0, sz, 0,
                0, 0, 0, 1)
        );
    }

    glm::mat4 RotateOZ(float radians)
    {
        return glm::transpose(
            glm::mat4(cos(radians), -sin(radians), 0, 0,
                sin(radians), cos(radians), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1)
        );
    }

    glm::mat4 RotateOY(float radians)
    {
        return glm::transpose(
            glm::mat4(cos(radians), 0, sin(radians), 0,
                0, 1, 0, 0,
                -sin(radians), 0, cos(radians), 0,
                0, 0, 0, 1)
        );
    }

    glm::mat4 RotateOX(float radians)
    {
        return glm::transpose(
            glm::mat4(1, 0, 0, 0,
                0, cos(radians), -sin(radians), 0,
                0, sin(radians), cos(radians), 0,
                0, 0, 0, 1)
        );
    }


    glm::mat4 Perspective(
        float fovy, float aspect, float zNear, float zFar)
    {

        return transpose(glm::mat4(1 / (tan(fovy / 2) * aspect), 0, 0, 0,
            0, 1 / tan(fovy / 2), 0, 0,
            0, 0, (-1) * (zFar + zNear) / (zFar - zNear), (-2) * zFar * zNear / (zFar - zNear),
            0, 0, -1, 0));

    }

    glm::mat4 View(
        const glm::vec3& cameraPosition,
        const glm::vec3& target,
        const glm::vec3& up)
    {
        glm::vec3 forward = glm::normalize(target - cameraPosition);

        glm::vec3 right = glm::normalize(glm::cross(forward, up));

        glm::vec3 cameraUp = glm::cross(right, forward);

        glm::mat4 view = glm::mat4(
            right.x, cameraUp.x, -forward.x, 0,
            right.y, cameraUp.y, -forward.y, 0,
            right.z, cameraUp.z, -forward.z, 0,
            0, 0, 0, 1
        );


        glm::mat4 translation = glm::mat4(1.0f);
        translation[3] = glm::vec4(-cameraPosition, 1.0f);

        return view * translation;
    }
};

