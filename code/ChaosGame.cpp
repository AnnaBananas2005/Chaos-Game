// Partners: Anna, Ted
// - Press keys 3-10 to change vertex count (resets current vertices/points)
// - Press R to reset vertices/points and start over

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cmath>

using namespace sf;
using namespace std;

// Return ratio for given vertex count n
static float ratioForVertices(int n)
{
    switch (n)
    {
    case 3: return 0.5f;
    case 4: return 0.5f;
    case 5: return 1.0f / 1.61803398875f; // 1/phi ~ 0.618
    case 6: return 2.0f / 3.0f;           // ~0.667
    case 7: return 0.692f;
    case 8: return 0.707f;
    case 9: return 0.742f;
    case 10:return 0.764f;
    default: return 0.5f;
    }
}

int main()
{
    VideoMode vm(1280, 720);
    RenderWindow window(vm, "Chaos Game");
    window.setFramerateLimit(60);

    srand(static_cast<unsigned int>(time(nullptr)));

	// Load font
    Font font;
    bool fontLoaded = font.loadFromFile("fonts/Roboto-Black.ttf");
    if (!fontLoaded) {
        cerr << "Warning: failed to load fonts/Roboto-Black.ttf. Text may not render." << endl;
    }

	// Instructions text
    Text instr;
    instr.setFont(font);
    instr.setCharacterSize(20);
    instr.setFillColor(Color::White);
    instr.setPosition(10.f, 10.f);

	// Initial settings
    int vertexCount = 3; // default triangle
    float ratio = ratioForVertices(vertexCount);

    vector<Vector2f> vertices;
    vector<Vector2f> points;

    bool collecting = true; // collecting vertices
    int lastSelectedVertex = -1;

	// Since the rendering is CPU bound and thus slow, generate multiple points per frame for faster results
    const int pointsPerFrame = 200;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
			// Keypresses dettection for controls
            if (event.type == Event::KeyPressed)
            {
                // Change polygon vertex count (keys 3..9, 0 -> 10)
                if (event.key.code >= Keyboard::Num3 && event.key.code <= Keyboard::Num9)
                {
                    vertexCount = (event.key.code - Keyboard::Num0); // Num3 -> 3, etc.
                    vertices.clear();
                    points.clear();
                    collecting = true;
                    lastSelectedVertex = -1;
                    ratio = ratioForVertices(vertexCount);
                }
                else if (event.key.code == Keyboard::Num0) // '0' => 10
                {
                    vertexCount = 10;
                    vertices.clear();
                    points.clear();
                    collecting = true;
                    lastSelectedVertex = -1;
                    ratio = ratioForVertices(vertexCount);
                }
                else if (event.key.code == Keyboard::R) // reset
                {
                    vertices.clear();
                    points.clear();
                    collecting = true;
                    lastSelectedVertex = -1;
                }
                else if (event.key.code == Keyboard::Escape)
                {
                    window.close();
                }
            }

            // Mouse input collection
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                float mx = static_cast<float>(event.mouseButton.x);
                float my = static_cast<float>(event.mouseButton.y);
                cout << "Click at: " << mx << ", " << my << endl;

                if (collecting)
                {
                    // Collect up to vertexCount vertices via first N clicks
                    if ((int)vertices.size() < vertexCount)
                    {
                        vertices.emplace_back(mx, my);
                        cout << "Vertex " << vertices.size() << " stored: " << mx << ", " << my << endl;
                        if ((int)vertices.size() == vertexCount)
                        {
                            cout << "Collected all vertices. Now click once to set starting point." << endl;
                        }
                    }
                    else
                    {
                        // If already collected, but still in collecting mode, first extra click becomes initial point
                        points.emplace_back(mx, my);
                        collecting = false; // start generating fractal
                        cout << "Starting point set: " << mx << ", " << my << endl;
                    }
                }
            }
        } // end event loop

        // Update instructions text
        {
            ostringstream ss;
            if (collecting)
            {
                ss << "Mode: Collecting vertices (" << vertices.size() << "/" << vertexCount << ").\n";
                ss << "Click " << (vertexCount - (int)vertices.size()) << " more point(s) to set polygon vertices.\n";
                ss << "After vertices, click once to set starting point.\n";
            }
            else
            {
                ss << "Mode: Running fractal (n=" << vertexCount << ", r=" << ratio << ").\n";
                ss << "Clicks ignored. Press R to reset or press 3..0 to change vertex count.\n";
            }
            ss << "Controls: 3-9 => polygon sides 3-9, 0 => 10, R => reset, Esc => quit";
            instr.setString(ss.str());
        }

        // Update - generate new points if we have an initial point and vertices
        if (!collecting && points.size() > 0 && vertices.size() >= (size_t)vertexCount)
        {
            for (int k = 0; k < pointsPerFrame; ++k)
            {
                int newRand = rand() % vertexCount;
                // For polygons >= 4 avoid choosing same vertex twice in a row (hint from rubric)
                if (vertexCount >= 4 && lastSelectedVertex >= 0)
                {
                    while (newRand == lastSelectedVertex)
                        newRand = rand() % vertexCount;
                }
                lastSelectedVertex = newRand;

                Vector2f vertex = vertices[newRand];
                Vector2f lastPt = points.back();
                Vector2f newPt;
                newPt.x = lastPt.x + (vertex.x - lastPt.x) * ratio;
                newPt.y = lastPt.y + (vertex.y - lastPt.y) * ratio;
                points.push_back(newPt);

                // prevent unbounded growth during extremely long runs in memory constrained environments:
                if (points.size() > 2000000) // 2 million guard
                {
                    points.erase(points.begin(), points.begin() + 100000); // trim oldest
                }
            }
        }

        // Draw
        window.clear(Color::Black);

        // draw vertices as small squares
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            RectangleShape rect(Vector2f(10.f, 10.f));
            rect.setOrigin(5.f, 5.f);
            rect.setPosition(vertices[i]);
            rect.setFillColor(Color::Cyan);
            window.draw(rect);
        }

        // draw points (fractal)
        for (size_t i = 0; i < points.size(); ++i)
        {
            CircleShape point(1.0f);
            point.setPosition(points[i]);
            point.setFillColor(Color::White);
            window.draw(point);
        }

        // draw UI text
		if (fontLoaded) // only draw if font loaded to avoid issues if it failed
            window.draw(instr);

        window.display();
    }

    return 0;
}
