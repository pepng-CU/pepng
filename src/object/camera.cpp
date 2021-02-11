#include "camera.hpp"
#include "../component/transform.hpp"
#include "../component/camera.hpp"
#include "../component/fps.hpp"

namespace pepng {
    std::shared_ptr<Object> makeCameraObj(std::shared_ptr<CameraTransform> transform, std::shared_ptr<Viewport> viewport, std::shared_ptr<Projection> projection) {
        auto camera = pepng::makeObject("Camera");
        
        camera->attachComponent(transform);
        camera->attachComponent(pepng::makeCamera(viewport, projection));

        if(Camera::cameras.size() == 1) {
            camera->attachComponent(pepng::makeFPS());
        }

        return camera;
    }
};