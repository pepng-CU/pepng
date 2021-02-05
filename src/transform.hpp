#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform {
    public:
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        Transform();

        Transform(const Transform &transform);

        Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

        Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

        glm::highp_mat4 getScaleMatrix(glm::highp_mat4 &matrix);

        glm::highp_mat4 getTranslateMatrix(glm::highp_mat4 &matrix);

        glm::highp_mat4 getRotationMatrix();

        glm::highp_mat4 getRotationMatrix(glm::highp_mat4 &matrix);

        /**
         * Gets the world matrix to be used for uniform matrix.
         */
        glm::highp_mat4 getWorldMatrix();

        /**
         * Gets the view matrix to be used for uniform matrix.
         */
        glm::highp_mat4 getViewMatrix();

        /**
         * Rotates using euler degree rotation relative to current rotation.
         * 
         * @param deltaYaw Rotation on X axis.
         * @param deltaPitch Rotation on Y axis.
         */
        void deltaRotate(float deltaYaw, float deltaPitch);

        /**
         * Delta translates relative to the local up, forward, right.
         * 
         * @param delta The delta translation to apply.
         */
        void relativeTranslate(glm::vec3 delta);

        /**
         * Gets the forward vector relative to current rotation.
         */
        glm::vec3 getForward();

        /**
         * Gets the up vector relative to current rotation.
         */
        glm::vec3 getUp();

        /**
         * Gets the right vector relative to current rotation.
         */
        glm::vec3 getRight();

        Transform operator+(const Transform &transform) {
            Transform ct = Transform(*this);

            ct.position += transform.position;
            ct.rotation *= transform.rotation;
            ct.scale.x *= transform.scale.x;
            ct.scale.y *= transform.scale.y;
            ct.scale.z *= transform.scale.z;

            return ct;
        }
};