#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

double FRAME_TIME = 0.7;

const unsigned MAX_SNAKE_LENGTH = 1000;
const int BOX_DIM = 100;

const unsigned WINDOW_WIDTH = 1600;
const unsigned WINDOW_HEIGHT = 900;

sf::RectangleShape snakeBody[MAX_SNAKE_LENGTH]; 
sf::Vector2f prevPos[MAX_SNAKE_LENGTH];
sf::Vector2f applePos;

unsigned indexOfLastSnakeElement = 0;

bool snakeHeadCollides()
{
    auto headPos = snakeBody[0].getPosition();

    if (headPos.x < 0 || headPos.x >= WINDOW_WIDTH)
    {
        return 1;
    }

    if (headPos.y < 0 || headPos.y >= WINDOW_HEIGHT)
    {
        return 1;
    }
    
    for (unsigned i = 1; i < indexOfLastSnakeElement; ++i)
    {
        auto curPos = snakeBody[i].getPosition();
        if (headPos.x == curPos.x && headPos.y == curPos.y)
        {
            return 1;
        }
    }

    return 0;
}

sf::Vector2f anyApplePos()
{
    sf::Vector2f pos;
    unsigned rectsX = WINDOW_WIDTH / BOX_DIM;
    unsigned rectsY = WINDOW_HEIGHT / BOX_DIM;
    
    unsigned mulX = rand()%rectsX;
    unsigned mulY = rand()%rectsY;

    pos.x = BOX_DIM * mulX;
    pos.y = BOX_DIM * mulY;

    return pos;
}

void placeAppleNotOnSnake()
{
    auto pos = anyApplePos();
    
    bool okay = 0;

    while(okay == 0)
    {
        okay = 1;

        for (unsigned i = 0; i < indexOfLastSnakeElement + 1; ++i)
        {
            auto curPos = snakeBody[i].getPosition();
            if (pos.x == curPos.x && pos.y == curPos.y)
            {
                pos = anyApplePos();
                okay = 0;
                break;
            }
        }

        if (okay == 1)
            break;
    }

    applePos = pos;
}

float lerp(float a, float b, float t)
{
    return ((1.0f - t) * a + t * b);
}


void onQuit()
{
    std::cout<<"POINTS:"<<indexOfLastSnakeElement<<std::endl;    
}


int main()
{
    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SNAKE!");
    
    for (auto&& e:snakeBody)
    {
        e.setFillColor(sf::Color::Green);
        e.setSize({BOX_DIM, BOX_DIM});
        e.setPosition({-1, -1});
    }

    snakeBody[0].setPosition({0,0});
    snakeBody[0].setFillColor(sf::Color::Yellow);
    
    placeAppleNotOnSnake();
    sf::RectangleShape apple;
    apple.setFillColor(sf::Color::Red);
    apple.setSize({BOX_DIM, BOX_DIM});
    apple.setPosition(applePos);

    sf::Clock clock;

    double elapsed = 0;
    double betweenApples = 0;
    
    sf::Vector2f movingDir(1, 0);

    while (window.isOpen())
    {
        double dt = clock.restart().asSeconds();
        elapsed += dt;
        betweenApples += dt;
                
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                onQuit();
                return 0;
            }
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                movingDir = {-1, 0};
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                movingDir = {1, 0};
            }
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                movingDir = {0, 1};
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                movingDir = {0, -1};
            }
        }

        float x = (float)(indexOfLastSnakeElement) / 20;
        
        float inter = (12.4*x) / (10*x +2.4);

        if (inter > 1) inter = 1;
        if (inter < 0) inter = 0;
        
        FRAME_TIME = lerp(0.3, 0.1, inter);

        if (elapsed > FRAME_TIME)
        {
            elapsed = 0;
        
            for (unsigned i = 1; i < MAX_SNAKE_LENGTH; ++i)
            {
                prevPos[i] = snakeBody[i-1].getPosition();
            }

            for (unsigned i = 1; i < MAX_SNAKE_LENGTH; ++i)
            {
                snakeBody[i].setPosition(prevPos[i]);
            }
            
            snakeBody[0].move(movingDir.x * BOX_DIM, -movingDir.y * BOX_DIM);

            if (snakeHeadCollides())
            {
                onQuit();
                return 0;
            }    
            
            if (snakeBody[0].getPosition().x == applePos.x && snakeBody[0].getPosition().y == applePos.y)
            {
                if (indexOfLastSnakeElement < MAX_SNAKE_LENGTH)
                {
                    ++indexOfLastSnakeElement;
                }

                sf::Color newColor;
                newColor.r = 0;
                newColor.b = 0;
                float t = betweenApples / 5;
                if (t > 1) t = 1;
                
                newColor.g = lerp(255, 100, t); 
                snakeBody[indexOfLastSnakeElement].setFillColor(newColor);

                placeAppleNotOnSnake();
                apple.setPosition(applePos);

                betweenApples = 0;
            }
            
        }
                        
        window.clear();

        for (unsigned i = 0; i < indexOfLastSnakeElement + 1; ++i)
        {
            window.draw(snakeBody[i]);
        }
        
        window.draw(apple);
            
        window.display();
    }

    return 0;
}
