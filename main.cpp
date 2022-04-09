#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int screen_width = 1024;
int screen_height = 768;

int roadW = 2000;  // road width (left to right)
int segL = 200;    // segment length (top to bottom)
float camD = 0.84; // camera depth

// scene colors
Color dark_grass = Color(0, 154, 0);
Color light_grass = Color(16, 200, 16);
Color white_rumble = Color(255, 255, 255);
Color black_rumble = Color(0, 0, 0);
Color dark_road = Color(105, 105, 105);
Color light_road = Color(107, 107, 107);

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
    int pos = 0;

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::Up))
            pos += 200;
        if (Keyboard::isKeyPressed(Keyboard::Down))
            pos -= 200;
        int startPos = pos / segL;

        app.clear();

        // draw road
        for (int n = startPos; n < startPos + 300; n++)
        {
            Line &current = lines[n % N];
            current.project(0, 1500, pos);

            Line prev = lines[(n - 1) % N]; // previous line

            Color grass = (n / 3) % 2 ? light_grass : dark_grass;
            Color rumble = (n / 3) % 2 ? white_rumble : black_rumble;
            Color road = (n / 3) % 2 ? light_road : dark_road;

            drawQuad(app, grass, 0, prev.Y, screen_width, 0, current.Y, screen_width);
            drawQuad(app, rumble, prev.X, prev.Y, prev.W * 1.2, current.X, current.Y, current.W * 1.2);
            drawQuad(app, road, prev.X, prev.Y, prev.W, current.X, current.Y, current.W);
        }

        app.display();
    }

    return 0;
}
