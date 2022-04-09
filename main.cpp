#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int width = 1024;
int height = 768;

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
    RenderWindow app(VideoMode(width, height), "Outrun Racing!");
    app.setFramerateLimit(60);

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
