#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <font_data.h>
#include <random>
#include <vector>
#include <chrono>
#include <iostream>
#include <cmath>

struct Ball {
    sf::Vector2f position;
    sf::Vector2f velocity = {0,0};
    float mass = 1;
    float radius;
    int sizeId;
    sf::Color color = sf::Color::Red;
};


int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    unsigned int windowWidth = 540;
    unsigned int windowHeight = 1070;
    unsigned int sidepanel = 300;
    sf::RenderWindow window(sf::VideoMode({windowWidth+sidepanel,windowHeight}), "Bouncing Balls Simulation", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    std::vector<Ball> balls;
    std::vector<Ball> removedBalls;
    std::vector<int> sizes = {15, 25, 45, 60, 80, 120, 160};
    std::vector<sf::Color> colors = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow, sf::Color::Cyan, sf::Color::Magenta, sf::Color::White};
    balls.reserve(30);
    // int numBalls = 0;

    srand(static_cast<unsigned int>(time(0)));
    // for (int i = 0; i < numBalls; ++i) {
    //     Ball ball;
    //     ball.position = {static_cast<float>(rand() % (windowWidth-40) + 40), static_cast<float>(rand() % (windowHeight-40) + 40)};
    //     ball.velocity = {static_cast<float>(rand() % 200 - 100), static_cast<float>(rand() % 200 - 100)};
    //     ball.sizeId = static_cast<float>(rand() % 3);
    //     ball.radius = sizes[ball.sizeId];
    //     // ball.mass = pow(ball.radius, 2) * 3.14f * 2;
    //     ball.mass = 1;
    //     ball.color = sf::Color(rand() % 256, rand() % 256, rand() % 256);
    //     balls.push_back(ball);
    // }

    sf::Clock clock;
    sf::Clock timeout;
    float dt;
    const sf::Vector2f g = {0.f, 981.f};
    float bounceDamp = 0.1f;
    float linearDamp = 0.001f;
    const float OFFSET = 0.0001f;
    const float MIN_SEP = 0.01f; 
    const float POS_COR = 0.8f;  
    unsigned int scorenum = 0;
    bool timeoutCheck = false;
    unsigned int numBallsOut;

    sf::Font font;
    if (!font.loadFromMemory(arial_ttf, arial_ttf_len )) {
        std::cerr << "Failed to load font\n";
        return -1;
    }

    sf::Text Lscore;
    sf::Text score;
    sf::Text Lnext;
    sf::Text gameOver;

    Lscore.setFont(font);
    Lscore.setCharacterSize(24);
    Lscore.setFillColor(sf::Color::Black);
    Lscore.setPosition(static_cast<float>(windowWidth + 20), 20.f);
    Lscore.setString("Score:");

    score.setFont(font);
    score.setCharacterSize(24);
    score.setFillColor(sf::Color::Black);
    score.setPosition(static_cast<float>(windowWidth + 20), 60.f);
    score.setString("0");

    Lnext.setFont(font);
    Lnext.setCharacterSize(24);
    Lnext.setFillColor(sf::Color::Black);
    Lnext.setPosition(static_cast<float>(windowWidth + 20), 100.f);
    Lnext.setString("Next Ball: ");

    gameOver.setFont(font);
    gameOver.setCharacterSize(48);
    gameOver.setFillColor(sf::Color::Red);
    gameOver.setPosition(static_cast<float>(windowWidth/2 - 100), static_cast<float>(windowHeight/2 - 50));
    gameOver.setString("Time till Death: 5.0");

    sf::RectangleShape rectangle({static_cast<float>(sidepanel), static_cast<float>(windowHeight)});
    rectangle.setPosition(static_cast<float>(windowWidth), 0.f);
    rectangle.setFillColor(sf::Color{150, 150, 150, 255});
    
    Ball nextBall;
    nextBall.position = {windowWidth+(static_cast<float>(sidepanel/2)), 250};
    nextBall.sizeId = static_cast<float>(rand() % 3);
    nextBall.radius = sizes[nextBall.sizeId];
    nextBall.color = colors[nextBall.sizeId];

    while (window.isOpen())
    {
        dt = clock.restart().asSeconds();
        const float MAX_DT = 1.0f/60.0f;  // Cap at 60 FPS
        dt = std::min(dt, MAX_DT);
        numBallsOut = 0;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && sf::Mouse::getPosition(window).x <= static_cast<int>(windowWidth)) {
                nextBall.position = {static_cast<float>(sf::Mouse::getPosition(window).x), 0.f + nextBall.radius};
                balls.push_back(nextBall);
                nextBall.position = {windowWidth+(static_cast<float>(sidepanel/2)), 250};
                nextBall.sizeId = static_cast<float>(rand() % 3);
                nextBall.radius = sizes[nextBall.sizeId];
                nextBall.color = colors[nextBall.sizeId];
            }
        }

        std::vector<char> removed(balls.size(), 0);

        //handle wall collisions
        for (size_t i = 0; i < balls.size(); ++i) {
            Ball& ball = balls[i];
            ball.velocity += g * dt;
            ball.position += ball.velocity * dt;
            ball.velocity -= ball.velocity * linearDamp;

            // Floor
            if (ball.position.y + ball.radius > windowHeight - OFFSET) {
                ball.position.y = windowHeight - ball.radius - OFFSET;
                ball.velocity.y = (-ball.velocity.y) * bounceDamp;
                if (std::abs(ball.velocity.y) < 0.0001f) ball.velocity.y = 0;
                if (std::abs(ball.velocity.x) < 0.0001f) ball.velocity.x = 0;
                ball.velocity.x *= (1.f - linearDamp*5.f);
            }
            // Ceiling
            if (ball.position.y - ball.radius < 0) {
                if (!timeoutCheck){
                    timeoutCheck = true;
                    timeout.restart();
                }
                numBallsOut++;
            }
            // Right wall
            if (ball.position.x + ball.radius > windowWidth - OFFSET) {
                ball.position.x = windowWidth - ball.radius - OFFSET;
                ball.velocity.x = (-ball.velocity.x) * bounceDamp;
            }
            // Left wall
            if (ball.position.x - ball.radius < 0 + OFFSET) {
                ball.position.x = 0 + ball.radius + OFFSET;
                ball.velocity.x = (-ball.velocity.x) * bounceDamp;
            }
        }
        if (numBallsOut == 0){
            timeoutCheck = false;
        }

        for (size_t i = 0; i < balls.size(); ++i) {
            if (removed[i]) continue;
            for (size_t j = i + 1; j < balls.size(); ++j) {
                if (removed[j]) continue;

                Ball &b1 = balls[i];
                Ball &b2 = balls[j];

                sf::Vector2f diff = b1.position - b2.position;
                float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                float overlap = (b1.radius + b2.radius) - dist;

                if (overlap > MIN_SEP) {
                    // merge when same size
                    if (b1.sizeId == b2.sizeId && b1.sizeId < static_cast<int>(sizes.size()-1)) {
                        if (b1.sizeId + 1 < static_cast<int>(sizes.size())) {
                            b1.sizeId++;
                            b1.radius = sizes[b1.sizeId];
                            b1.position.x += (diff.x * 0.5f);
                            b1.position.y += (diff.y * 0.5f);
                            b1.color = colors[b1.sizeId];
                            scorenum += 10 * b1.sizeId;
                            score.setString(std::to_string(scorenum));
                        }
                        removed[j] = 1; // mark b2 for removal
                        continue;
                    }
                    // calculate impulse
                    sf::Vector2f normal = (dist > 0.0f) ? (diff / dist) : sf::Vector2f(1.f, 0.f);

                    float totalMass = b1.mass + b2.mass;
                    float ratio1 = b1.mass / totalMass;
                    float ratio2 = b2.mass / totalMass;

                    b1.position += normal * overlap * ratio2 * POS_COR;
                    b2.position -= normal * overlap * ratio1 * POS_COR;

                    float relVel = (b1.velocity.x - b2.velocity.x) * normal.x + (b1.velocity.y - b2.velocity.y) * normal.y;
                    if (relVel > 0) continue;

                    float jimp = -(1.f + bounceDamp) * relVel;
                    jimp /= (1.f / b1.mass) + (1.f / b2.mass);

                    sf::Vector2f impulse = jimp * normal;
                    b1.velocity += impulse / b1.mass;
                    b2.velocity -= impulse / b2.mass;
                }
            }
        }
        
        // compact removed balls (one pass)
        bool anyRemoved = std::any_of(removed.begin(), removed.end(), [](char c){ return c != 0; });
        if (anyRemoved) {
            std::vector<Ball> compacted;
            compacted.reserve(balls.size());
            for (size_t i = 0; i < balls.size(); ++i) {
                if (!removed[i]) compacted.push_back(std::move(balls[i]));
            }
            balls.swap(compacted);
        }
        
        //draw stuff here
        window.clear(sf::Color{211, 185, 87, 255});
        window.draw(rectangle);
        window.draw(Lscore);
        window.draw(score); 
        window.draw(Lnext);
        sf::CircleShape circle(nextBall.radius);
        circle.setFillColor(nextBall.color);
        circle.setPosition(nextBall.position - sf::Vector2f(nextBall.radius, nextBall.radius));
        window.draw(circle);
        for (const auto& ball : balls) {
            sf::CircleShape circle(ball.radius);
            circle.setFillColor(ball.color);
            circle.setPosition(ball.position - sf::Vector2f(ball.radius, ball.radius));
            window.draw(circle);
        }

        if (timeoutCheck){
            if (timeout.getElapsedTime().asSeconds() >= 5){
                timeoutCheck = false;
                score.setString("0");
                balls.clear();
                gameOver.setString("You Lost!\nScore: " + std::to_string(scorenum));
                scorenum = 0;
                window.draw(gameOver);
                window.display();
                sf::sleep(sf::seconds(2));
            }
            else{
                gameOver.setString("Time till Death: " + std::to_string(5 - static_cast<int>(timeout.getElapsedTime().asSeconds())));
                window.draw(gameOver);
            }
        }
        window.display();


    }
    return 0;
}


