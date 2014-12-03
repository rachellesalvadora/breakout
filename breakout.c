/*
* breakout.c
*
* Computer Science 50
* Problem Set 4
*/

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// lives
#define LIVES 3

#define BLOCK_W 10
#define BLOCK_H 10

#define B_DIMENSION 0.03
#define B_DY

#define BRICK_W (WIDTH - (2*10)) / COLS
#define BRICK_H 2*10

#define XPADDLE (WIDTH/2) - (60/2) - 2
#define YPADDLE 570



// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    
    GObject collisionObject;
    
    waitForClick();

    double bdx = .7;
    double bdy = 2.0;
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // TODO
        while (true)
        {

        // THANKIES http://d2o9nyf4hwsci4.cloudfront.net/2013/fall/lectures/5/m/src5m/spl/doc/gevents.html#Type:GMouseEvent
        GEvent event = getNextEvent(MOUSE_EVENT);

        if(event != NULL)
        {
            if (getEventType(event) == MOUSE_MOVED)
            {
                double x = getX(event) - getWidth(paddle) / 2;
                setLocation(paddle, x, YPADDLE);
            }
        }
        
        
        // move circle along x-axis
        move(ball, bdx, bdy);

        // bounce from right
        if ((getX(ball) + getWidth(ball) >= WIDTH) || getX(ball) <= 0)
        {
            bdx *= -1;
        }
        
        // bounce from left
        else if (getY(ball) <= 0)
        {
            bdy *= -1;
        }
        
        else if ((getY(ball) + getHeight(ball) >= HEIGHT))
        {
            lives--;
            setLocation(ball, 200 - 10, 300 - 10);
            setLocation(paddle, XPADDLE, YPADDLE);
            bdx = B_DIMENSION;
            bdy = B_DY(B_DIMENSION);
            waitForClick();
        }

    collisionObject = detectCollision(window, ball);
    
    if (collisionObject != NULL && strcmp(getType(collisionObject), "GRect") == 0)
    {     
        bdy *=  -1;   
        if (collisionObject != paddle)
        {
            removeGWindow(window, collisionObject);
            bricks--;
            points++;
            updateScoreboard(window, label, points);
            printf("bricks %d points: %d\n", bricks, points);
            
            if (points >= 45)
            {
            waitForClick();
            }
        }

    }
    }
    }
    
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
    
 }   

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // TODO
    string brickColors[] = {"RED", "ORANGE", "YELLOW", "BLUE", "GREEN"};
    int colorIndex = 0;
    
    int lastX = 350;
    int lastY = 100;
    
    for (int y = 20; y <= lastY; y += BRICK_H)
    {
        for (int x = 25; x <= lastX; x += BRICK_W)
       {
       GRect brick = newGRect(x+2, y+2, BRICK_W-4, BRICK_H-8);
       setColor(brick,brickColors[colorIndex]);
       setFilled(brick, true);
       add(window, brick);
       }
           colorIndex++;
    }
    

}


/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // TODO
    
    int x = (WIDTH / 2) - 10;
    int y = (HEIGHT / 2) - 10;
    
    GOval ball = newGOval(x, y, 10*2, 10*2);
    setFilled(ball, true);
    
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // TODO
    int x = (WIDTH / 2) - 60 / 2;
    int y = YPADDLE;
    
    GRect paddle = newGRect(x, y, 60, 10 );
    setFilled(paddle, true);
    setColor(paddle, "BLUE");
    add(window, paddle);
    
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // TODO
    GLabel label = newGLabel("0");
    setFont(label,"SansSerif-40");
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
    setColor(label, "RED");

    add(window, label);
    
    return label;
}


/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * 10, y);
    
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * 10);
    
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * 10, y + 2 * 10);
    
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
