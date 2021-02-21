#include "camera.hpp"
#include "../component/transform.hpp"
#include "../component/camera.hpp"
#include "../component/fps.hpp"
#include "../component/light.hpp"

namespace pepng {
    std::shared_ptr<Object> makeCameraObj(std::shared_ptr<CameraTransform> transform, std::shared_ptr<Camera> camera) {
        auto cameraObj = pepng::makeObject("Camera");
        
        cameraObj->attachComponent(transform);
        cameraObj->attachComponent(camera);

        if(Camera::cameras.size() == 1) {
            cameraObj->attachComponent(pepng::makeFPS());
        }

        return cameraObj;
    }

    std::shared_ptr<Object> makeCameraObj(std::shared_ptr<CameraTransform> transform, std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) {
        return makeCameraObj(transform, pepng::makeCamera(viewport, projection));
    }
};