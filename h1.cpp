//Header files 
#include <iostream>
#include <graphics.h>//To intialize graphics
#include <conio.h>
#include <stdlib.h>//Standard Library
#include <windows.h>//EXCEPTION

// Declaration of GameObject Class
class GameObject {
protected:
    int x, y;//position:x,y
    int w, h;//size:w(width of object),h(height of object)
public:
//Use of Constructor
    GameObject(int _x, int _y, int _w, int _h) {
        x = _x; y = _y; w = _w; h = _h;
    }
    int getX() { return x; }//To return value of x
    int getY() { return y; }//To return value of y
    int getW() { return w; }//To return value of w
    int getH() { return h; }//To return value of h
};

//Declaration of Bird Class
//Concept of Inheritence
//Bird Class(Child Class) inherets from GameObject(Parent Class)
class Bird : public GameObject {
    int vy;//vy=VERTICAL VELOCITY
    int gravity;//gravity=TO GET BIRD FALL DOWN
    int flap_power;//flap_power=So that the bird wil jump or flap
public:
    Bird(int _x, int _y) : GameObject(_x, _y, 20, 20) {
        vy = 0;//The Bird should not mobve up or down at the start so velocity=0
        gravity = 1;//The small Gravity value so that the bird falls down
        flap_power = -8;//The flap_power is -8 because Negative number means bird goes upwards as we press sapacebar
    }
    //function to make the bird jump
    void jump() { vy = flap_power; }
    //function to update the position of bird
    void update() {
        vy += gravity;//Increases speed because gravity makes the bird fall faster
        y += vy;//move bird up and down
        //to stop the bird to move out of the screen
        if (y < 0) y = 0;//It ensures the bird cannot go above the top edge of the screen.
    }
     //fuction to draw the bird
    void draw() {
        setcolor(YELLOW);
        setfillstyle(SOLID_FILL, YELLOW);
        fillellipse(x, y, w / 2, h / 2);//draw circle as bird
    }
};
// Pipe Class
class Pipe : public GameObject {
    int gap;
    int speed;
public:
    Pipe(int _x, int _w, int _gap) : GameObject(_x, 0, _w, 0) {
        speed = 4;
        gap = _gap;
        reset();//Randomize height
    }
    void reset() {
        y = rand() % 200 + 50;//top pipe height
    }
    void update() {
        x -= speed;
        if (x + w < 0) {
            x = 600;
            reset();
        }
    }
    void draw(int screenh) {
        setcolor(GREEN);
        setfillstyle(SOLID_FILL, GREEN);
        //top pipe
        bar(x, 0, x + w, y);
         //bottom pipe
        bar(x, y + gap, x + w, screenh - 10);
    }
    int checkCollision(Bird* bird, int screenh) {
        int bx = bird->getX();
        int by = bird->getY();
        int bw = bird->getW();
        int bh = bird->getH();
        //Check Horizontal overlap
        if (bx + bw / 2 > x && bx - bw / 2 < x + w) {
             //If bird is Not in the gap vertically
            if (by - bh / 2 < y || by + bh / 2 > y + gap) {
                return 1;
            }
        }
        return 0;
    }
    int passedBy(Bird* bird) {
        return (bird->getX() == x + w);
    }
};
// Declaration of Game Class
class Game {
    Bird* bird;
    Pipe* pipe;
    int screenw, screenh;//screenw-width of the screen
    //height of screen
    int score;
    void* background;  // background image buffer pointer
    int highscore;
public:
    Game(int w, int h, void* bg) {
        screenw = w;
        screenh = h;
        bird = new Bird(100, h / 2);
        pipe = new Pipe(w, 40, 100);//Pipe starting from right
        score = 0;
        background = bg;
        highscore=0;
    }

    void drawScore() {
         //Showing the score and updating the score
        char text[20];
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 5);
        sprintf(text, "Score: %d", score);
        outtextxy(10, 10, text);
    }

    // ADDED CODE START: Show "Bird hit the pipe!" message
    void showGameOverMessage() {
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
        outtextxy(screenw / 2 - 100, screenh / 2 - 20, (char*)"Bird hit the pipe!");
        delay(1000); // Show message for 1 second
    }
    // ADDED CODE END

    void run() {
        while (1) {
            //Check key press so to check input
            //check if any key is pressed
            if (kbhit()) {
                char key = getch();//Read the key is pressed
                if (key == 27) break;//27=ESC KEY//break stat.for if any spacebar is pressed
                if (key == ' ') bird->jump();//Bird jumps up
            }

            bird->update();
            pipe->update();

            cleardevice();//clear oldscreen
               //to draw bird at new position
            // Draw the cached background from memory, no lag now
            putimage(0, 0, background, COPY_PUT);

            bird->draw();//Draw ground line at bottom
            pipe->draw(screenh);
            drawScore();
            line(0, screenh - 10, screenw, screenh - 10);

            if (bird->getY() > screenh - 20 || pipe->checkCollision(bird, screenh)) {

                if(score>highscore){
                    highscore=score;
                }

                // ADDED CODE START: Show "Bird hit the pipe!" message first
                showGameOverMessage();
                // ADDED CODE END

                cleardevice(); // Clear old frame before showing final screen
                putimage(0, 0, background, COPY_PUT);

                settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
                outtextxy(screenw / 2 - 60, screenh / 2, (char*)"Game Over!");

                // ADDED CODE START: Show score slowly after game over message
                char hstext[30];
                for(int i = 0; i <= score; i++) {
                    sprintf(hstext, "Score: %d", i);
                    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
                    outtextxy(screenw /2 - 50, screenh/2 + 30, hstext);
                    delay(200); // delay to show score slowly
                }
                sprintf(hstext ,"High score:%d", highscore);
                outtextxy(screenw /2-70,screenh/2+60,hstext);
                // ADDED CODE END

                getch();
                break;
            }

            if (pipe->passedBy(bird)) score++;
            delay(40);
        }
    }
     //use of  Destructor
    ~Game() {
        delete bird;
        delete pipe;
    }
};

// Main Function
int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, const_cast<char*>(""));

    // Load background image once
    readimagefile("background.bmp", 0, 0, 600, 400);

    // Allocate memory and capture the background image into buffer
    void* background = malloc(imagesize(0, 0, 600, 400));
    getimage(0, 0, 600, 400, background);

    cleardevice();

    Game game(600, 400, background);
    game.run();

    free(background);  // Free allocated memory

    getch();
    closegraph();
    return 0;
}

