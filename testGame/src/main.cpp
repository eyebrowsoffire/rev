#include <rev/Camera.h>
#include <rev/Engine.h>
#include <rev/Environment.h>
#include <rev/IActor.h>
#include <rev/IKeyboardListener.h>
#include <rev/IMouseListener.h>
#include <rev/Light.h>
#include <rev/MtlFile.h>
#include <rev/NurbsCurve.h>
#include <rev/ObjFile.h>
#include <rev/ProgramFactory.h>
#include <rev/Scene.h>
#include <rev/SceneView.h>
#include <rev/TrackModel.h>
#include <rev/WavefrontHelpers.h>
#include <rev/Window.h>
#include <rev/physics/Gravity.h>
#include <rev/physics/Particle.h>
#include <rev/physics/System.h>

#include <glm/ext.hpp>

#include <chrono>
#include <cmath>

using namespace rev;

namespace {
constexpr glm::vec3 kCubeVertices[] = {
    // Front face
    { -1.0, -1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { -1.0, 1.0, 1.0 },
    { -1.0, -1.0, 1.0 },
    { 1.0, -1.0, 1.0 },
    { 1.0, 1.0, 1.0 },

    // Back face
    { -1.0, -1.0, -1.0 },
    { -1.0, 1.0, -1.0 },
    { 1.0, 1.0, -1.0 },
    { -1.0, -1.0, -1.0 },
    { 1.0, 1.0, -1.0 },
    { 1.0, -1.0, -1.0 },

    // Top face
    { -1.0, 1.0, 1.0 },
    { 1.0, 1.0, -1.0 },
    { -1.0, 1.0, -1.0 },
    { -1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, 1.0, -1.0 },

    // Bottom face
    { -1.0, -1.0, 1.0 },
    { -1.0, -1.0, -1.0 },
    { 1.0, -1.0, -1.0 },
    { -1.0, -1.0, 1.0 },
    { 1.0, -1.0, -1.0 },
    { 1.0, -1.0, 1.0 },

    // Left face
    { -1.0, -1.0, -1.0 },
    { -1.0, 1.0, 1.0 },
    { -1.0, 1.0, -1.0 },
    { -1.0, -1.0, -1.0 },
    { -1.0, -1.0, 1.0 },
    { -1.0, 1.0, 1.0 },

    // Right face
    { 1.0, -1.0, -1.0 },
    { 1.0, 1.0, -1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, -1.0, -1.0 },
    { 1.0, 1.0, 1.0 },
    { 1.0, -1.0, 1.0 }
};

std::vector<glm::vec3>
buildFlatNormalsForVertices(gsl::span<const glm::vec3> vertices)
{
    Expects(vertices.size() % 3 == 0);
    size_t primitiveCount = vertices.size() / 3;
    std::vector<glm::vec3> normals;
    for (size_t primitiveIndex = 0; primitiveIndex < primitiveCount;
         primitiveIndex++) {
        size_t vertexIndex = primitiveIndex * 3;
        glm::vec3 edge1 = vertices[vertexIndex] - vertices[vertexIndex + 1];
        glm::vec3 edge2 = vertices[vertexIndex + 2] - vertices[vertexIndex + 1];
        glm::vec3 normal = glm::cross(edge1, edge2);

        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);
    }

    return normals;
}

class RotatingCameraController : public rev::IActor {
public:
    RotatingCameraController(std::shared_ptr<rev::Camera> camera)
        : _camera(std::move(camera))
    {
    }

    void tick(rev::Environment& environment, rev::Duration) override
    {
        using FloatSeconds = std::chrono::duration<float>;

        constexpr float radius = 2.0f;
        float t = FloatSeconds(environment.getTotalElapsedTime()).count();

        float y = 1.0f * sin(t / 4.0f);
        float x = radius * cos(t);
        float z = radius * sin(t);

        _camera->setPosition({ x, y, z });
    }

    void kill(rev::Environment&) override {}

private:
    std::shared_ptr<rev::Camera> _camera;
};

class UserCameraController : public rev::IMouseListener,
                             public rev::IKeyboardListener,
                             public rev::IActor {
public:
    UserCameraController(std::shared_ptr<rev::Camera> camera,
        const rev::Point<double>& initialPosition)
        : _camera(std::move(camera))
        , _lastPosition(initialPosition)
        , _zSpeed(0.0f)
    {
    }

    void buttonPressed(rev::MouseButton) override {}

    void buttonReleased(rev::MouseButton) override {}

    void moved(rev::Point<double> position) override
    {
        constexpr float sensitivity = 0.01f;
        float xDelta = static_cast<float>(position.x - _lastPosition.x) * sensitivity;
        float yDelta = static_cast<float>(position.y - _lastPosition.y) * sensitivity;

        glm::vec3 currentPosition = _camera->getPosition();
        glm::vec3 target = _camera->getTarget();
        glm::vec3 cameraVector = target - currentPosition;
        glm::mat4 transform(1.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 leftVector = glm::cross(cameraVector, upVector);
        transform = glm::rotate(transform, -xDelta, upVector);
        transform = glm::rotate(transform, -yDelta, leftVector);

        glm::vec4 newCameraVector = transform * glm::vec4(cameraVector, 1.0f);
        newCameraVector /= newCameraVector.w;

        glm::vec3 newTarget = currentPosition + glm::vec3(newCameraVector.x, newCameraVector.y, newCameraVector.z);

        _camera->setTarget(newTarget);

        _lastPosition = position;
    }

    void scrolled(double, double) override {}

    void keyPressed(rev::KeyboardKey key) override
    {
        switch (key) {
        case rev::KeyboardKey::W:
            _zSpeed += 1.0f;
            break;
        case rev::KeyboardKey::S:
            _zSpeed -= 1.0f;
            break;
        case rev::KeyboardKey::A:
            _xSpeed -= 1.0f;
            break;
        case rev::KeyboardKey::D:
            _xSpeed += 1.0f;
            break;
        default:
            return;
        }
    }

    void keyReleased(rev::KeyboardKey key) override
    {
        switch (key) {
        case rev::KeyboardKey::W:
            _zSpeed -= 1.0f;
            break;
        case rev::KeyboardKey::S:
            _zSpeed += 1.0f;
            break;
        case rev::KeyboardKey::A:
            _xSpeed += 1.0f;
            break;
        case rev::KeyboardKey::D:
            _xSpeed -= 1.0f;
            break;
        default:
            return;
        }
    }

    void tick(rev::Environment&, rev::Duration elapsedTime) override
    {
        if ((abs(_zSpeed) <= 0.01f) && (abs(_xSpeed) <= 0.01f)) {
            return;
        }

        using FloatSeconds = std::chrono::duration<float>;

        glm::vec4 movementDirection = glm::vec4(_xSpeed, 0.0f, -_zSpeed, 0.0f);

        movementDirection = glm::inverse(_camera->getViewMatrix()) * movementDirection;

        glm::vec3 movementVector(movementDirection.x, movementDirection.y,
            movementDirection.z);
        movementVector = glm::normalize(movementVector);

        constexpr float speedFactor = 9.0f;
        movementVector *= speedFactor * static_cast<FloatSeconds>(elapsedTime).count();

        auto position = _camera->getPosition();
        auto target = _camera->getTarget();
        _camera->setPosition(position + movementVector);
        _camera->setTarget(target + movementVector);
    }

    void kill(rev::Environment&) override {}

private:
    std::shared_ptr<rev::Camera> _camera;
    rev::Point<double> _lastPosition{ 0.0, 0.0 };
    float _zSpeed;
    float _xSpeed;
};

class BikeController : public rev::IActor, public rev::IKeyboardListener {
public:
    BikeController(std::shared_ptr<rev::physics::Particle> particle,
        std::shared_ptr<rev::CompositeObject> sceneObject)
        : _particle(std::move(particle))
        , _sceneObject(std::move(sceneObject))
    {
        Expects(_particle != nullptr);
        Expects(_sceneObject != nullptr);
    }

    void tick(rev::Environment&, rev::Duration elapsedTime) override
    {
        if (_thrustersOn) {
            rev::physics::Force<glm::vec3> force(0.0f, 7000.0f, 0.0f);

            _particle->addImpulse(force * rev::physics::durationToPhysicsTime(elapsedTime));
        }

        const auto& currentPosition = _particle->getPosition().getValue();
        if (currentPosition.y < 0.0f) {
            auto newPosition = currentPosition;
            newPosition.y = 0.0f;
            _particle->setPosition(newPosition);

            auto newVelocity = _particle->getVelocity().getValue();
            newVelocity.y = 0.0f;
            _particle->setVelocity(newVelocity);
        }

        glm::mat4 identity;
        _sceneObject->transform = glm::translate(identity, _particle->getPosition().getValue());
    }

    void kill(rev::Environment& environment) override {}

    void keyPressed(rev::KeyboardKey key) override
    {
        if (key == rev::KeyboardKey::Space) {
            _thrustersOn = true;
        }
    }

    void keyReleased(rev::KeyboardKey key) override
    {
        if (key == rev::KeyboardKey::Space) {
            _thrustersOn = false;
        }
    }

private:
    std::shared_ptr<rev::physics::Particle> _particle;
    std::shared_ptr<rev::CompositeObject> _sceneObject;
    bool _thrustersOn = false;
};

} // namespace

int main(void)
{
    rev::Engine engine;
    auto window = engine.createWindow("Test Game", { 1280, 720 });
    window->makeCurrent();

    auto sceneView = engine.createSceneView();
    sceneView->setOutputSize({ 320, 180 });
    // sceneView->setOutputSize({1280, 720});

    auto scene = engine.createScene();
    sceneView->setScene(scene);
    window->setSceneView(sceneView);

    auto verticesSpan = gsl::span<const glm::vec3>(kCubeVertices);
    auto normals = buildFlatNormalsForVertices(verticesSpan);

    rev::ObjFile meshFile("assets/hoverbike.obj");
    rev::MtlFile materialsFile("assets/hoverbike.mtl");
    rev::ProgramFactory factory;
    auto objectGroup = rev::createObjectGroupFromWavefrontFiles(factory, meshFile,
        materialsFile);
    auto object = objectGroup->addObject();
    scene->addObjectGroup(objectGroup);

    float width = 3.0f;
    size_t segmentCount = 100;
    float halfRoot2 = sqrt(2.0f) / 2.0f;
    WeightedControlPoint<glm::vec3> controlPoints[] = {
        // Wide 90 degree turn
        { { -20.0f, 0.0f, 0.0f }, 1.0f },
        { { -20.0f, 0.0f, 20.0f }, halfRoot2 },
        { { 0.0, 0.0f, 20.0f }, 1.0f },

        // Straight line
        { { 50.0f, 0.0f, 20.0f }, 1.0f },

        // Right 180 degree turn, climbing uphill
        { { 60.0f, 0.0f, 20.0f }, halfRoot2 },
        { { 60.0f, 10.0f, 10.0f }, 1.0f },
        { { 60.0f, 20.0f, 0.0f }, halfRoot2 },
        { { 50.0f, 20.0f, 0.0f }, 1.0f },

        // Some sort of curvy line back toward the start
        { { 20.0f, 20.0f, 0.0f }, 1.0f },
        { { 0.0f, 0.0f, -20.0f }, 1.0f },

        // Wide 90 degree turn back to the start
        { { -20.0f, 0.0f, -20.0f }, halfRoot2 },
        { { -20.0f, 0.0f, 0.0f }, 1.0f },
    };
    float knots[] = {
        0.0,
        0.0,
        0.0,
        0.1,
        0.1,
        0.1,
        0.2,
        0.2,
        0.3,
        0.3,
        0.4,
        0.5,
        0.6,
        0.6,
        0.6,
    };

    NurbsCurve<glm::vec3> curve(3, knots, controlPoints);

    auto trackGroup = std::make_shared<rev::SceneObjectGroup<rev::TrackModel>>(factory, curve, width, segmentCount);
    scene->addObjectGroup(trackGroup);

    auto physicsSystem = std::make_shared<rev::physics::System>();
    auto bikeParticle = physicsSystem->addParticle();
    bikeParticle->setMass(500.0f);

    auto gravity = physicsSystem->getGravity();
    gravity->setAcceleration(glm::vec3(0.0f, -10.f, 0.0f));
    gravity->addParticle(bikeParticle);

    auto bikeController = std::make_shared<BikeController>(bikeParticle, object);

    auto yellowLight = scene->addLight();
    yellowLight->setPosition(glm::vec3(4.0f, 3.0f, 3.0f));
    yellowLight->setBaseColor(glm::vec3(1.0f, 1.0f, 0.8f));

    auto blueLight = scene->addLight();
    blueLight->setPosition(glm::vec3(-1.5f, -2.0f, 1.5f));
    blueLight->setBaseColor(glm::vec3(0.2f, 0.2f, 1.0f));

    auto orangeLight = scene->addLight();
    orangeLight->setPosition(glm::vec3(3.0f, 0.75f, -2.5f));
    orangeLight->setBaseColor(glm::vec3(1.0f, 0.3f, 0.0f));

    auto camera = sceneView->getCamera();
    camera->setTarget({ 0.0f, 0.0f, 0.0f });
    camera->setPosition({ 4.0f, 4.0f, 4.0f });
    camera->setAspectRatio(1280.0f / 720.0f);

    auto cameraController = std::make_shared<UserCameraController>(
        std::move(camera), window->getMousePosition());
    window->addMouseListener(cameraController);
    window->addKeyboardListener(cameraController);
    window->addKeyboardListener(bikeController);

    auto environment = engine.createEnvironment();
    environment->addActor(cameraController);
    environment->addActor(physicsSystem);
    environment->addActor(bikeController);

    environment->play();

    while (!window->wantsClose()) {
        engine.update();
    }
    return 0;
}
