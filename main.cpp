#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

int main() {
    const unsigned int W = 600;
    const unsigned int H = 900;

    // Thêm settings để hỗ trợ alpha
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(W, H), "Mars - Jupiter with Textures", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    constexpr double PI = 3.14159265358979323846;
    const sf::Vector2f center(W / 2.f, H / 2.f);

    // THU NHỎ ĐỒNG ĐỀU để phù hợp với short 9:16
    const float rMars    = 52.f;    // Giảm từ 72 → 52
    const float rJupiter = 136.f;   // Giảm từ 209 → 126 (giữ nguyên tỉ lệ)

    const double golden = (1.0 + std::sqrt(5.0)) / 2.0;
    const double ratio = 5.0 + golden;
    const double speedMars    = 2.0 * PI;
    const double speedJupiter = speedMars / ratio;

    const int linesPerRevolution = 800;

    // Tải texture từ file PNG
    sf::Texture sunTexture, marsTexture, jupiterTexture;

    // Load texture bình thường (nếu PNG đã có alpha channel)
    if (!sunTexture.loadFromFile("sun.png")) {
        return -1;
    }
    sunTexture.setSmooth(true);

    if (!marsTexture.loadFromFile("mars.png")) {
        return -1;
    }
    marsTexture.setSmooth(true);

    if (!jupiterTexture.loadFromFile("jupiter.png")) {
        return -1;
    }
    jupiterTexture.setSmooth(true);

    // Quỹ đạo (thu nhỏ tương ứng)
    sf::CircleShape orbitMars(rMars);
    orbitMars.setPosition(center);
    orbitMars.setOrigin(rMars, rMars);
    orbitMars.setFillColor(sf::Color::Transparent);
    orbitMars.setOutlineThickness(1.f);
    orbitMars.setOutlineColor(sf::Color(255,255,255,90));

    sf::CircleShape orbitJupiter(rJupiter);
    orbitJupiter.setPosition(center);
    orbitJupiter.setOrigin(rJupiter, rJupiter);
    orbitJupiter.setFillColor(sf::Color::Transparent);
    orbitJupiter.setOutlineThickness(1.f);
    orbitJupiter.setOutlineColor(sf::Color(255,255,255,70));

    // Tạo sprite
    sf::Sprite sun;
    sun.setTexture(sunTexture);
    sun.setOrigin(sunTexture.getSize().x / 2.f, sunTexture.getSize().y / 2.f);
    sun.setPosition(center);
    sun.setScale(32.f / sunTexture.getSize().x, 32.f / sunTexture.getSize().y);
    sun.setColor(sf::Color(255, 255, 255, 255)); // Đảm bảo alpha đầy đủ

    sf::Sprite mars;
    mars.setTexture(marsTexture);
    mars.setOrigin(marsTexture.getSize().x / 2.f, marsTexture.getSize().y / 2.f);
    mars.setScale(14.f / marsTexture.getSize().x, 14.f / marsTexture.getSize().y);
    mars.setColor(sf::Color(255, 255, 255, 255));

    sf::Sprite jupiter;
    jupiter.setTexture(jupiterTexture);
    jupiter.setOrigin(jupiterTexture.getSize().x / 2.f, jupiterTexture.getSize().y / 2.f);
    jupiter.setScale(26.f / jupiterTexture.getSize().x, 26.f / jupiterTexture.getSize().y);
    jupiter.setColor(sf::Color(255, 255, 255, 255));

    std::vector<sf::VertexArray> lines;
    double t = 0.0;
    int lastStep = -1;
    sf::Clock clock;

    // Biến cho chuyển động tự quay của sao Mộc
    float jupiterRotation = 0.0f;
    const float jupiterRotationSpeed = 10.0f;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed || e.key.code == sf::Keyboard::Escape) window.close();
            if (e.type == sf::Event::KeyPressed && (e.key.code == sf::Keyboard::C || e.key.code == sf::Keyboard::Space))
                lines.clear(), t = 0, lastStep = -1;
        }

        float dt = clock.restart().asSeconds();
        t += dt * 0.86;

        // Cập nhật chuyển động tự quay của sao Mộc
        jupiterRotation += jupiterRotationSpeed * dt;

        double angleMars    = speedMars    * t;
        double angleJupiter = speedJupiter * t;

        double fraction = std::fmod(angleJupiter, 2*PI) / (2*PI);
        int currentStep = static_cast<int>(fraction * linesPerRevolution);

        if (currentStep != lastStep && currentStep >= 0) {
            sf::Vector2f posMars(center.x + rMars    * std::cos(angleMars),
                                 center.y + rMars    * std::sin(angleMars));
            sf::Vector2f posJupiter(center.x + rJupiter * std::cos(angleJupiter),
                                    center.y + rJupiter * std::sin(angleJupiter));

            sf::Vector2f dir = posJupiter - posMars;
            float len = std::hypot(dir.x, dir.y);
            if (len > 4.f) {
                sf::Vector2f start = posMars    + dir * (7.f / len);
                sf::Vector2f end   = posJupiter - dir * (13.f / len);

                sf::VertexArray line(sf::Lines, 2);
                line[0].color = sf::Color(255,255,255,42);
                line[1].color = sf::Color(230,245,255,46);
                line[0].position = start;
                line[1].position = end;
                lines.push_back(line);
            }
            lastStep = currentStep;
        }

        sf::Vector2f posMars(center.x + rMars * std::cos(angleMars), center.y + rMars * std::sin(angleMars));
        sf::Vector2f posJupiter(center.x + rJupiter * std::cos(angleJupiter), center.y + rJupiter * std::sin(angleJupiter));
        mars.setPosition(posMars);
        jupiter.setPosition(posJupiter);

        jupiter.setRotation(jupiterRotation);

        window.clear(sf::Color(0,0,12));
        window.draw(orbitMars);
        window.draw(orbitJupiter);
        for (const auto& l : lines) window.draw(l);

        // Dây hiện tại
        sf::Vector2f dir = posJupiter - posMars;
        float len = std::hypot(dir.x, dir.y);
        if (len > 4.f) {
            sf::VertexArray curr(sf::Lines, 2);
            curr[0].position = posMars + dir * (7.f/len);
            curr[1].position = posJupiter - dir * (13.f/len);
            curr[0].color = sf::Color(255,255,255,180);
            curr[1].color = sf::Color(200,230,255,200);
            window.draw(curr);
        }

        window.draw(sun);
        window.draw(mars);
        window.draw(jupiter);
        window.display();
    }
    return 0;
}
