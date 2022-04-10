#include <SFML/Graphics.hpp>
#include <cmath>

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
    float scale, curve, spriteX, clip;
    Sprite sprite;

    Line() { spriteX = curve = x = y = z = 0; }

    // from game pos to screen pos
    void project(int camX, int camY, int camZ)
    {
        scale = camD / (z - camZ);
        X = (1 + scale * (x - camX)) * screen_width / 2;
        Y = (1 - scale * (y - camY)) * screen_height / 2;
        W = scale * roadW * screen_width / 2;
    }

    // draw a sprite in this Line position
    void drawSprite(RenderWindow &app)
    {
        Sprite s = sprite;
        int w = s.getTextureRect().width;
        int h = s.getTextureRect().height;

        float destX = X + scale * spriteX * screen_width / 2;
        float destY = Y + 4;
        float destW = w * W / 266;
        float destH = h * W / 266;

        destX += destW * spriteX; // offsetX
        destY += destH * (-1);    // offsetY

        float clipH = destY + destH - clip;
        if (clipH < 0)
            clipH = 0;

        if (clipH >= destH)
            return;
        s.setTextureRect(IntRect(0, 0, w, h - h * clipH / destH));
        s.setScale(destW / w, destH / h);
        s.setPosition(destX, destY);
        app.draw(s);
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

    Texture bg;
    bg.loadFromFile("images/bg.png");
    bg.setRepeated(true);
    Sprite sBackground(bg);
    sBackground.setTextureRect(IntRect(0, 0, 5000, 411));
    sBackground.setPosition(-2000, 0);

    Texture t[50];
    Sprite object[50];
    for (int i = 1; i <= 7; i++)
    {
        t[i].loadFromFile("images/" + to_string(i) + ".png");
        t[i].setSmooth(true);
        object[i].setTexture(t[i]);
    }

    // create road lines for each segment
    vector<Line> lines;
    for (int i = 0; i < 1600; i++)
    {
        Line line;
        line.z = i * segL;

        // curves segment
        if (i > 300 && i < 700)
            line.curve = 0.5;
        if (i > 1100)
            line.curve = -0.7;

        // after 750 it is uphill and downhill in sin() form
        if (i > 750)
            line.y = sin(i / 30.0) * 1500;

        // Sprites segments
        if (i < 300 && i % 20 == 0)
        {
            line.spriteX = -2.5;
            line.sprite = object[5];
        }
        if (i % 17 == 0)
        {
            line.spriteX = 2.0;
            line.sprite = object[6];
        }
        if (i > 300 && i % 20 == 0)
        {
            line.spriteX = -0.7;
            line.sprite = object[4];
        }
        if (i > 800 && i % 20 == 0)
        {
            line.spriteX = -1.2;
            line.sprite = object[1];
        }
        if (i == 400)
        {
            line.spriteX = -1.2;
            line.sprite = object[7];
        }

        lines.push_back(line);
    }
    int N = lines.size();
    int pos = 0;
    int playerX = 0;
    int playerY = 1500; // control camera height

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::Right))
            playerX += 200;
        if (Keyboard::isKeyPressed(Keyboard::Left))
            playerX -= 200;

        if (Keyboard::isKeyPressed(Keyboard::Up))
            pos += 200;
        if (Keyboard::isKeyPressed(Keyboard::Down))
            pos -= 200;
        int startPos = pos / segL;

        //control camera height
        if (Keyboard::isKeyPressed(Keyboard::W))
            playerY += 100;
        if (Keyboard::isKeyPressed(Keyboard::S))
            playerY -= 100;

        float x = 0, dx = 0; // curve offset on x axis

        int camH = 1500 + lines[startPos].y + playerY;
        int maxy = screen_height;

        // loop the circut from start to finish
        while (pos >= N * segL)
            pos -= N * segL;
        while (pos < 0)
            pos += N * segL;

        app.clear();
        app.draw(sBackground);

        // draw road
        for (int n = startPos; n < startPos + 300; n++)
        {
            Line &current = lines[n % N];
            // loop the circut from start to finish = pos - (n >= N ? N * segL : 0)
            current.project(playerX - x, camH, pos - (n >= N ? N * segL : 0));
            x += dx;
            dx += current.curve;

            current.clip = maxy; // offset sprite position
            // don't draw "above ground"
            if (current.Y >= maxy)
                continue;
            maxy = current.Y;

            Line prev = lines[(n - 1) % N]; // previous line

            Color grass = (n / 3) % 2 ? light_grass : dark_grass;
            Color rumble = (n / 3) % 2 ? white_rumble : black_rumble;
            Color road = (n / 3) % 2 ? light_road : dark_road;

            drawQuad(app, grass, 0, prev.Y, screen_width, 0, current.Y, screen_width);
            drawQuad(app, rumble, prev.X, prev.Y, prev.W * 1.2, current.X, current.Y, current.W * 1.2);
            drawQuad(app, road, prev.X, prev.Y, prev.W, current.X, current.Y, current.W);
        }

        // draw objects
        for (int n = startPos + 300; n > startPos; n--)
            lines[n % N].drawSprite(app);

        app.display();
    }

    return 0;
}
