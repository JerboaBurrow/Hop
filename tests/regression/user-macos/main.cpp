#include "main.h"

int main(int argv, char ** argc)
{

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;
    conf.COCOA_RETINA = false;

    jGL::DesktopDisplay display(glm::ivec2(resX, resY), "Shape", conf);

    glewInit();
    
    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(display.getRes()));

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    camera.setPosition(0.0f, 0.0f);

    jLog::Log log;

    high_resolution_clock::time_point tic, tock;
    double rdt = 0.0;

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(1);

    std::shared_ptr<jGL::ShapeRenderer> circles = jGLInstance->createShapeRenderer
    (
        32
    );

    std::vector<std::shared_ptr<jGL::Shape>> shapes;

    RNG rng;

    for (unsigned i = 0; i < 64; i++)
    {
        shapes.push_back
        (
            std::make_shared<jGL::Shape>
            (
                jGL::Transform(rng.nextFloat(), rng.nextFloat(), 0.0, 0.1f),
                glm::vec4(rng.nextFloat(), rng.nextFloat(), rng.nextFloat(), 1.0)
            )
        );

        circles->add(shapes[i], std::to_string(i));
    }

    circles->setProjection(camera.getVP());

    std::shared_ptr<jGL::Shader> shader = std::make_shared<jGL::GL::glShader>(vertexShader, fragmentShader);

    shader->use();

    double delta = 0.0;
    double dt = 1.0/600.0;

    while (display.isOpen())
    {
        tic = high_resolution_clock::now();

        jGLInstance->beginFrame();

            jGLInstance->clear();

            for (unsigned i = 0; i <shapes.size(); i++)
            {
                auto tr = circles->getTransform(std::to_string(i));
                circles->getShape(std::to_string(i))->update
                (
                    jGL::Transform(tr.x+dt*(rng.nextFloat()-0.5), tr.y+dt*(rng.nextFloat()-0.5), tr.theta, tr.scale)
                );
            }

            circles->draw(shader);

            delta = 0.0;
            for (int n = 0; n < 60; n++)
            {
                delta += deltas[n];
            }
            delta /= 60.0;
            
            std::stringstream debugText;

            double mouseX, mouseY;
            display.mousePosition(mouseX,mouseY);

            debugText << "Delta: " << fixedLengthNumber(delta,6)
                    << " ( FPS: " << fixedLengthNumber(1.0/delta,4) 
                    << ")\n"
                    << "Render draw time: \n" 
                    << "   " << fixedLengthNumber(rdt, 6) << "\n"
                    << "Mouse (" << fixedLengthNumber(mouseX,4) 
                    << "," 
                    << fixedLengthNumber(mouseY,4) 
                    << ")\n";

            jGLInstance->text(
                debugText.str(),
                glm::vec2(64.0f, resY-64.0f),
                0.5f,
                glm::vec4(0.0f,0.0f,0.0f,1.0f)
            );

            if (frameId == 30)
            {
                if (log.size() > 0)
                {
                    std::cout << log << "\n";
                }
            }

        jGLInstance->endFrame();

        display.loop();

        tock = high_resolution_clock::now();

        deltas[frameId] = duration_cast<duration<double>>(tock-tic).count();
        frameId = (frameId+1) % 60;
            
    }

    jGLInstance->finish();

    return 0;
}