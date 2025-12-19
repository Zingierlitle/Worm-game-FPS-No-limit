#include "glfw3.h"
#include <windows.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

struct Vec2 { int x, y; };

const int GRID_W = 40;
const int GRID_H = 30;
const int CELL_SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

void drawCell(int gx, int gy) {
    float x = (float)gx / GRID_W * 2 - 1;
    float y = (float)gy / GRID_H * 2 - 1;
    float w = 2.0f / GRID_W;
    float h = 2.0f / GRID_H;

    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
}

int main() {
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    srand((unsigned)time(nullptr));

    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(GRID_W * CELL_SIZE, GRID_H * CELL_SIZE, "Worm Game | Score: 0 | FPS: 0", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); 

    std::vector<Vec2> snake = {{GRID_W / 2, GRID_H / 2}};
    Direction dir = RIGHT;
    double moveTimer = 0.0;
    double moveDelay = 0.25;

    Vec2 food = {rand() % GRID_W, rand() % GRID_H};

    bool paused = false;
    bool escLast = false;

    double lastFPS = glfwGetTime();
    int frames = 0;
    int currentFPS = 0;

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();

        bool escNow = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
        if (escNow && !escLast) paused = !paused;
        escLast = escNow;

        if (!paused) {
            if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && dir != DOWN) dir = DOWN;
            if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && dir != UP) dir = UP;
            if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && dir != RIGHT) dir = LEFT;
            if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && dir != LEFT) dir = RIGHT;
        }

        if (!paused && now - moveTimer >= moveDelay) {
            moveTimer = now;
            Vec2 head = snake.front();
            switch (dir) {
                case UP: head.y--; break;   
                case DOWN: head.y++; break;  
                case LEFT: head.x--; break;
                case RIGHT: head.x++; break;
            }

            if (head.x < 0 || head.x >= GRID_W || head.y < 0 || head.y >= GRID_H) {
                glfwSetWindowShouldClose(window, true);
            }

            bool collided = false;
            for (auto& s : snake) if (s.x == head.x && s.y == head.y) collided = true;
            if (collided) {
                snake = {{GRID_W / 2, GRID_H / 2}};
                dir = RIGHT;
                food = {rand() % GRID_W, rand() % GRID_H};
            }


            snake.insert(snake.begin(), head);

            if (head.x == food.x && head.y == food.y) {
                food = {rand() % GRID_W, rand() % GRID_H};
            } else {
                snake.pop_back();
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        for (auto& s : snake) drawCell(s.x, s.y);
        drawCell(food.x, food.y);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();

        frames++;
        if (now - lastFPS >= 1.0) {
            currentFPS = frames;
            frames = 0;
            lastFPS = now;

            std::string title;
            if (paused) {
                title = "Game paused | Score: " + std::to_string((int)snake.size() - 1) +
                        " | FPS: " + std::to_string(currentFPS);
            } else {
                title = "game | Score: " + std::to_string((int)snake.size() - 1) +
                        " | FPS: " + std::to_string(currentFPS);
            }
            glfwSetWindowTitle(window, title.c_str());
        }
    }

    glfwTerminate();
    return 0;
}
