#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "component.hpp"

class Transform : public Component {
    public:
        glm::vec3 position;
        glm::vec3 scale;
        glm::mat4 parentMatrix;

        static std::shared_ptr<Transform> makeTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

        static std::shared_ptr<Transform> makeTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

        static std::shared_ptr<Transform> copyTransform(std::shared_ptr<Transform> transform);

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
         * @param degDelta Rotation on XYZ axis.
         */
        void deltaRotate(glm::vec3 degDelta);

        /**
         * Delta translates relative to the local up, forward, right.
         * 
         * @param delta The delta translation to apply.
         */
        void relativeTranslate(glm::vec3 delta);

        void setRotation(glm::vec3 rotation);

        void setRotation(const Transform& transform);

        glm::vec3 getEuler();

        virtual glm::quat getRotation();

        /**
         * Gets the forward vector relative to current rotation.
         */
        virtual glm::vec3 getForward();

        /**
         * Gets the up vector relative to current rotation.
         */
        virtual glm::vec3 getUp();

        /**
         * Gets the right vector relative to current rotation.
         */
        virtual glm::vec3 getRight();

        virtual void imgui() override;

        friend std::ostream& operator<<(std::ostream& os, const Transform& transform);
    protected:
        Transform(const Transform &transform);

        Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

        Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

        Transform(
            glm::vec3 position, 
            glm::quat rotationX, 
            glm::quat rotationY,
            glm::quat rotationZ, 
            glm::vec3 scale
        );

        glm::quat rotationX;
        glm::quat rotationY;
        glm::quat rotationZ;
};

std::ostream& operator<<(std::ostream& os, const Transform& transform);

/**
 * Class that fixes the transform for the camera.
 *
 * Use this class instead of the normal transform for the camera.
 */
class CameraTransform : public Transform {  
    public:
        static std::shared_ptr<CameraTransform> makeCameraTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

        static std::shared_ptr<CameraTransform> makeCameraTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

        static std::shared_ptr<CameraTransform> copyCameraTransform(std::shared_ptr<CameraTransform> transform);

        virtual glm::quat getRotation() override;

        virtual glm::vec3 getForward() override;

        virtual glm::vec3 getUp() override;

        virtual glm::vec3 getRight() override;
    private:
        CameraTransform(const Transform &transform);

        CameraTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

        CameraTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

};

namespace pepng {
    std::shared_ptr<Transform> makeTransform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));

    std::shared_ptr<Transform> makeTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

    std::shared_ptr<CameraTransform> makeCameraTransform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));

    std::shared_ptr<CameraTransform> makeCameraTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

    std::shared_ptr<Transform> copyTransform(std::shared_ptr<Transform> transform);

    std::shared_ptr<CameraTransform> copyCameraTransform(std::shared_ptr<CameraTransform> transform);
}