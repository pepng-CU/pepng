#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "component.hpp"

/**
 * Component to store position, scale, and rotation.
 * 
 * Helper classes to generate the various matricies for these components.
 */
class Transform : public Component {
    public:
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 shear;
        glm::mat4 parent_matrix;

        static std::shared_ptr<Transform> make_transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear);

        static std::shared_ptr<Transform> make_transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear);

        glm::highp_mat4 scale_matrix(glm::highp_mat4 &matrix);

        glm::highp_mat4 translation_matrix(glm::highp_mat4 &matrix);

        glm::highp_mat4 rotation_matrix();

        glm::highp_mat4 rotation_matrix(glm::highp_mat4 &matrix);

        glm::highp_mat4 shear_matrix(glm::highp_mat4 &matrix);

        /**
         * Gets the world matrix to be used for uniform matrix.
         */
        glm::highp_mat4 world_matrix();

        /**
         * Gets the view matrix to be used for uniform matrix.
         */
        glm::highp_mat4 view_matrix();

        /**
         * Rotates using euler degree rotation relative to current rotation.
         * 
         * @param degDelta Rotation on XYZ axis.
         */
        void delta_rotate(glm::vec3 degDelta);

        /**
         * Delta translates relative to the local up, forward, right.
         * 
         * @param delta The delta translation to apply.
         */
        void relative_translate(glm::vec3 delta);

        /**
         * Sets rotation to euler rotation.
         */
        void set_rotation(glm::vec3 rotation);

        /**
         * Copies rotation of other transform.
         */
        void copy_rotation(const Transform& transform);

        /**
         * Gets euler rotation.
         */
        glm::vec3 euler();

        /**
         * Gets rotation quaternion.
         */
        virtual glm::quat rotation();

        /**
         * Gets the forward vector relative to current rotation.
         */
        virtual glm::vec3 forward();

        /**
         * Gets the up vector relative to current rotation.
         */
        virtual glm::vec3 up();

        /**
         * Gets the right vector relative to current rotation.
         */
        virtual glm::vec3 right();

        virtual void imgui() override;

        friend std::ostream& operator<<(std::ostream& os, const Transform& transform);
    protected:
        glm::quat rotationX;
        glm::quat rotationY;
        glm::quat rotationZ;

        Transform(const Transform &transform);

        Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear);

        Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear);

        Transform(
            glm::vec3 position, 
            glm::quat rotationX, 
            glm::quat rotationY,
            glm::quat rotationZ, 
            glm::vec3 scale,
            glm::vec3 shear
        );

        virtual Transform* clone_implementation() override;
};

std::ostream& operator<<(std::ostream& os, const Transform& transform);

/**
 * Class that fixes the transform for the camera.
 *
 * Use this class instead of the normal transform for the camera.
 */
class CameraTransform : public Transform {  
    public:
        static std::shared_ptr<CameraTransform> make_camera_transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear);

        static std::shared_ptr<CameraTransform> make_camera_transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear);

        virtual glm::quat rotation() override;

        virtual glm::vec3 forward() override;

        virtual glm::vec3 up() override;

        virtual glm::vec3 right() override;
    protected:
        virtual CameraTransform* clone_implementation() override;
    
    private:
        CameraTransform(const CameraTransform &transform);

        CameraTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 shear);

        CameraTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear);

};

namespace pepng {
    std::shared_ptr<Transform> make_transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 shear = glm::vec3(0.0f));

    std::shared_ptr<Transform> make_transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear);

    std::shared_ptr<CameraTransform> make_camera_transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 shear = glm::vec3(0.0f));

    std::shared_ptr<CameraTransform> make_camera_transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale, glm::vec3 shear);
}