#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int screen_width = 1024;
int screen_height = 768;

int roadW = 2000;  // road width (left to right)
int segL = 200;    // segment length (top to bottom)
float camD = 0.84; // camera depth

struct Line
{
    float x, y, z; // game position (3D space)
    float X, Y, W; // screen position (2D space)
    float scale;

    Line() { x = y = z = 0; }

    // from game pos to screen pos
    void project(int camX, int camY, int camZ)
    {
        scale = camD / (z - camZ);
        X = (1 + scale * (x - camX)) * screen_width / 2;
        Y = (1 - scale * (y - camY)) * screen_height / 2;
        W = scale * roadW * screen_width / 2;
    }
};

// Draws a quadratic polygon on screen
//
void drawQuad(RenderWindow &w, Color c, int x1, int y1, int w1, int x2, int y2, int w2)
{
    ConvexShape shape(4.f);
    shape.setFillColor(c);
    shape.setPoint(0, Vector2f(x1 - w1, y1));
    shape.setPoint(1, Vector2f(x2 - w2, y2));
    shape.setPoint(2, Vector2f(x2 + w2, y2));
    shape.setPoint(3, Vector2f(x1 + w1, y1));
    w.draw(shape);
}

int main()
{
    RenderWindow app(VideoMode(screen_width, screen_height), "Outrun Racing!");
    app.setFramerateLimit(60);

    // create road lines for each segment
    vector<Line> lines;
    for (int i = 0; i < 1600; i++)
    {
        Line line;
        line.z = i * segL;

        lines.push_back(line);
    }
    int N = lines.size();

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        app.clear();
        drawQuad(app, Color::Green, 500, 500, 200, 500, 300, 100);
        app.display();
    }

    return 0;
}
