#include <ctime>
#include <cstdlib>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


#if 1   // set this to 0 if dont want to print debugging info
#define D(x) x
#else
#define D(x)
#endif


// defining NO_WALLS will allow snake to pass through walls
#define NO_WALLS

class vec2d 
{
public:
    int x,y;
    vec2d(int x_, int y_): x(x_), y(y_)
    {}
};

enum Direction {Left, Right, Up, Down};


// Global Vars
const int scale = 20;       // scale of drawing
vec2d win_dim(500, 500);    // window dimensions (height & width must be multiple of scale)
const int fps = 8;          // render fps

unsigned score = 0;         // Player score
const int score_increment_amt = 5;  // amount by which score should be incremented

vec2d starting_loc(10, 10);     // starting location of snake
Direction starting_dir(Right);  // starting direction of snake


class Food
{
private:
    vec2d loc;  // current location of food

    // generate random number between two numbers (including min, max)
    inline int _randint(int min, int max)  
    {
        return rand()%(max-min + 1) + min;
    }

public:
    Food(): loc(100, 100)
    {
        srand(time(NULL));
    }

    // Draw Food
    void draw(sf::RenderWindow *win)
    {
        sf::CircleShape c((float)scale/2);
        c.setPosition((sf::Vector2f(floor((float)loc.x), floor((float)loc.y))));
        c.setFillColor(sf::Color(255, 0, 0));
        win->draw(c);
    }

    // Pick new location for food
    void picknewloc()
    {
        int newx = _randint(1, win_dim.x-1);
        int newy = _randint(1, win_dim.y-1);
        loc.x = scale*(newx/scale);
        loc.y = scale*(newy/scale);
    }

    // Get food location
    vec2d get_loc() {return loc;}
};

class Snake
{
private:
    vec2d head_loc;                 // coordinates of head
    int speed;                      // snake speed
    Direction dir;                  // snake direction
    std::vector<vec2d> tail;        // history of head coordinates

public:
    Snake():
        head_loc(scale*4, scale*4),
        speed(scale*1), 
        dir(Right)
    {}

    // update 
    bool update(Food *f)
    {
        // collision detection [Boundry]
        #ifndef NO_WALLS
        if(head_loc.x+scale > win_dim.x || head_loc.x < 0 || head_loc.y+scale > win_dim.y || head_loc.y < 0)
        {
            std::cout << "\nCollided with wall!" << std::endl;
            return false;
        }
        #endif
        
        // collision detection [Self]
        for(unsigned i=0; i<tail.size(); i++)
        {
            if(tail[i].x == head_loc.x && tail[i].y == head_loc.y)
            {
                std::cout << "\nLOL! You munched your own tail" << std::endl;
                return false;
            }
        }

        // collision detection [Food]
        if(f->get_loc().x == head_loc.x && f->get_loc().y == head_loc.y )
        {
            _eaten();
            f->picknewloc();
        }

        // update history
        if(tail.size()>0)
        {
            for(unsigned i=0; i<tail.size()-1; i++) // shift all elements
                tail[i] = tail[i+1];
            tail[tail.size()-1] = head_loc;         // edit last element
        }
        
        // Update head location
        switch(dir)
        {
            case Left:
                head_loc.x -= speed; break;
            case Right:
                head_loc.x += speed; break;
            case Up:
                head_loc.y -= speed; break;
            case Down:
                head_loc.y += speed; break;
            default:
                break;
        }

        #ifdef NO_WALLS
        // Teleport
        if(head_loc.x+scale > win_dim.x)
            head_loc.x = 0;
        else if(head_loc.x < 0)
            head_loc.x = win_dim.x-scale;

        if(head_loc.y+scale > win_dim.y)
            head_loc.y = 0;
        else if(head_loc.y < 0)
            head_loc.y = win_dim.y-scale;
        #endif

        return true;
    }

    // Draw snake
    void draw(sf::RenderWindow *win)
    {
        // Draw head
        sf::RectangleShape head_shape(sf::Vector2f((float)scale, (float)scale));
        head_shape.setPosition(sf::Vector2f(floor((float)head_loc.x), floor((float)head_loc.y)));
        head_shape.setFillColor(sf::Color(150, 50, 250));
        win->draw(head_shape);

        // Draw tail
        sf::RectangleShape tail_shape(sf::Vector2f((float)scale-2.f, (float)scale-2.f));
        for(unsigned i=0; i<tail.size(); i++)
        {
            tail_shape.setPosition(sf::Vector2f(floor((float)tail[i].x+1.f), floor((float)tail[i].y+1.f)));
            win->draw(tail_shape);
        }
    }

    // Set direction of travel
    void set_direction(Direction newdir)
    {
        // Block opposite direction of movement
        if(newdir == Left && dir == Right)
            return;
        else if(newdir == Right && dir == Left)
            return;
        else if(newdir == Up && dir == Down)
            return;
        else if(newdir == Down && dir == Up)
            return;
        
        dir = newdir;
    }

    vec2d get_head_loc()
    {return head_loc;}

private:
    // callback function, called if food is eaten
    void _eaten()
    {
        tail.push_back(vec2d(win_dim.x+50, win_dim.y+50));    // increase tail size (insert garbage element)
        score += score_increment_amt;
    }
};



int main()
{
    sf::RenderWindow window(sf::VideoMode(win_dim.x, win_dim.y), "Snake");
    window.setFramerateLimit(fps);

    bool paused = false;
    Snake s;
    Food f;
    
    while (window.isOpen()) // game loop
    {
        // check if window closed
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        // Check keboard
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            break;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            paused = !paused;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            s.set_direction(Left);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            s.set_direction(Right);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            s.set_direction(Up);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            s.set_direction(Down);
        
        printf("\r Score: %d\t ", score);
        D(printf("Food:(%d,%d)\t Head(%d, %d)       ", f.get_loc().x, f.get_loc().y, s.get_head_loc().x, s.get_head_loc().y);)
        std::cout<<std::flush;

        if(paused)
        {
            // display red window border if paused
            int thickness = 2;
            sf::RectangleShape border(sf::Vector2f(win_dim.x-2*thickness, win_dim.y-2*thickness));
            border.setPosition(sf::Vector2f((float)thickness, (float)thickness));
            border.setOutlineColor(sf::Color(255, 0, 0));
            border.setFillColor(sf::Color(255, 255, 255, 0));
            border.setOutlineThickness(2.0);
            window.draw(border);
        }
        else
        {
            window.clear();
            
            // Draw food
            f.draw(&window);

            // Draw snake
            s.draw(&window);
            
            if(s.update(&f) == false)
                break;
        }
        
        // Display
        window.display();
    }

    std::cout << "Your score is :" << score << std::endl;
    return 0;
}