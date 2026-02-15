#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>

using namespace std;

#define SCREEN_W 600
#define SCREEN_H 400
#define GROUND_Y (SCREEN_H - 10)

// ---------------- GAME OBJECT ----------------
class GameObject {
protected:
    int x, y, w, h;
public:
    GameObject(int _x, int _y, int _w, int _h) {
        x = _x; y = _y; w = _w; h = _h;
    }
    int getX() { return x; }
    int getY() { return y; }
    int getW() { return w; }
    int getH() { return h; }
};

// ---------------- BIRD ----------------
class Bird : public GameObject {
    int vy, gravity, flap_power;
public:
    Bird(int _x, int _y) : GameObject(_x,_y,20,20) {
        vy = 0;
        gravity = 1;
        flap_power = -8;
    }

    void jump() { vy = flap_power; }

    void update() {
        vy += gravity;
        y += vy;
        if (y < 0) y = 0;
    }

    void draw() {
        setcolor(YELLOW);
        setfillstyle(SOLID_FILL,YELLOW);
        fillellipse(x,y,w/2,h/2);

        // eye
        setcolor(BLACK);
        setfillstyle(SOLID_FILL,BLACK);
        fillellipse(x+5,y-3,2,2);

        // wing
        arc(x-2,y,200,340,8);
    }
};

// ---------------- PIPE ----------------
class Pipe : public GameObject {
    int gap, speed;
    bool passed;

public:
    Pipe(int _x,int _w,int _gap) : GameObject(_x,0,_w,0) {
        gap = _gap;
        speed = 4;
        reset();
    }

    void reset() {
        y = rand()%200 + 50;
        passed = false;
    }

    void update() {
        x -= speed;
        if(x+w < 0) {
            x = SCREEN_W;
            reset();
        }
    }

    void increaseDifficulty() {
        if(speed < 10) speed++;
    }

    void draw() {
        setfillstyle(SOLID_FILL,GREEN);

        // top pipe
        bar(x,0,x+w,y);
        bar(x-5,y-20,x+w+5,y);

        // bottom pipe
        bar(x,y+gap,x+w,GROUND_Y);
        bar(x-5,y+gap,x+w+5,y+gap+20);
    }

    bool checkCollision(Bird* bird) {
        int bx = bird->getX();
        int by = bird->getY();
        int r = bird->getW()/2;

        if(bx+r > x && bx-r < x+w) {
            if(by-r < y || by+r > y+gap)
                return true;
        }
        return false;
    }

    bool isPassed(Bird* bird) {
        if(!passed && bird->getX() > x+w) {
            passed = true;
            return true;
        }
        return false;
    }
};

// ---------------- GAME ----------------
class Game {
    Bird* bird;
    Pipe* pipe;
    int score, highscore;
    void* background;

public:
    Game(void* bg) {
        background = bg;
        highscore = 0;
    }

    void drawScore() {
        char text[20];
        settextstyle(DEFAULT_FONT,HORIZ_DIR,2);
        sprintf(text,"Score: %d",score);
        outtextxy(10,10,text);
    }

    void startScreen() {
        cleardevice();
        settextstyle(DEFAULT_FONT,HORIZ_DIR,3);
        outtextxy(SCREEN_W/2-140,SCREEN_H/2-40,"FLAPPY BIRD");

        settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
        outtextxy(SCREEN_W/2-120,SCREEN_H/2+10,"Press SPACE to start");

        while(getch()!=' ');
    }

    void showGameOver() {
        cleardevice();
        putimage(0,0,background,COPY_PUT);

        settextstyle(DEFAULT_FONT,HORIZ_DIR,2);
        outtextxy(SCREEN_W/2-60,SCREEN_H/2-20,"GAME OVER");

        char text[40];

        sprintf(text,"Score : %d",score);
        outtextxy(SCREEN_W/2-60,SCREEN_H/2+20,text);

        sprintf(text,"High Score : %d",highscore);
        outtextxy(SCREEN_W/2-80,SCREEN_H/2+50,text);

        outtextxy(SCREEN_W/2-120,SCREEN_H/2+90,"Press any key to restart");
        getch();
    }

    void run() {

        while(true) {

            startScreen();

            score = 0;
            bird = new Bird(100,SCREEN_H/2);
            pipe = new Pipe(SCREEN_W,40,100);

            while(true) {

                if(kbhit()) {
                    char key = getch();
                    if(key==27) return;
                    if(key==' ') bird->jump();
                }

                bird->update();
                pipe->update();

                cleardevice();
                putimage(0,0,background,COPY_PUT);

                bird->draw();
                pipe->draw();

                // ground
                setfillstyle(SOLID_FILL,BROWN);
                bar(0,GROUND_Y,SCREEN_W,SCREEN_H);

                drawScore();

                if(bird->getY()>GROUND_Y || pipe->checkCollision(bird)) {

                    if(score > highscore)
                        highscore = score;

                    delete bird;
                    delete pipe;

                    showGameOver();
                    break;
                }

                if(pipe->isPassed(bird)) {
                    score++;
                    pipe->increaseDifficulty();
                }

                delay(40);
            }
        }
    }
};

// ---------------- MAIN ----------------
int main() {

    int gd = DETECT, gm;
    initgraph(&gd,&gm,(char*)"");

    readimagefile("background.bmp",0,0,SCREEN_W,SCREEN_H);

    void* background = malloc(imagesize(0,0,SCREEN_W,SCREEN_H));
    getimage(0,0,SCREEN_W,SCREEN_H,background);

    cleardevice();

    Game game(background);
    game.run();

    free(background);
    closegraph();
    return 0;
}
