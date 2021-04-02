#include "camera.hpp"
#include "../component/transform.hpp"
#include "../component/camera.hpp"
#include "../component/fps.hpp"
#include "../component/light.hpp"

namespace pepng {
    std::shared_ptr<Object> make_camera_object(std::shared_ptr<CameraTransform> transform, std::shared_ptr<Camera> camera) {
        auto cameraObj = pepng::make_object("Camera");
        
        cameraObj->attach_component(transform);
        cameraObj->attach_component(camera);

        if(Camera::cameras.size() == 1) {
            cameraObj->attach_component(pepng::make_fps());
        }

        return cameraObj;
    }

    std::shared_ptr<Object> make_camera_object(std::shared_ptr<CameraTransform> transform, std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) {
        return make_camera_object(transform, pepng::make_camera(viewport, projection));
    }
};