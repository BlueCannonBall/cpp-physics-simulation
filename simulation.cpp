#include <map>
#include <iostream>
#include <GL/freeglut.h>
#include <math.h>

// Math functions
double getpercent(double amount, double max, double outof) {
    return (outof / max) * amount;
};
double randomRage() {
    return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
};
double ranMixMax(double min, double max) {
    return min + randomRage() * (max - min);
};
double getDistance(double xA, double yA, double xB, double yB) { 
	double xDiff = xA - xB; 
	double yDiff = yA - yB; 
	return sqrt(xDiff * xDiff + yDiff * yDiff);
};

// Objects
class Vector2d {
    public:
        double x;
        double y;
    void set(double wantedX, double wantedY) {
        x = wantedX;
        y = wantedY;
    };
};
class Entity {
    public:
        Vector2d position;
        Vector2d velocity;
        float radius;
    void set(double x, double y, double vx, double xy, float wantedRadis) {
        position.set(x, y);
        velocity.set(vx, xy);
        radius = wantedRadis;
    };
};
class EntityProjection {
    public:
        Vector2d position;
        double radius;
    void set(double x, double y, double r) {
        position.x = x;
        position.y = y;
        radius = r;
    };
};
class World {
    public:
        int width = 1000;
        int height = 1000;
        int entityLimit = 10;
        int entityCount = 0;
        int nextID = 1;
        double collisionStrength = 0.01;
        std::map<int, Entity> world;
    void createEntity() {
        Entity taret;
        taret.set(
            ranMixMax(0, width), // X
            ranMixMax(0, height), // Y
            ranMixMax(-1, 1), // Velocity X
            ranMixMax(-1, 1), // Velocity X
            50 // Radius
        );
        world.insert(std::pair<int, Entity>(nextID++, taret));
        entityCount++;
    };
};
// Main game
World game;

// Physics functions:
bool getCollision(Entity entity, Entity target) {
    return (getDistance(entity.position.x, entity.position.y, target.position.x, target.position.y) < entity.radius + target.radius);
};

// Drawing functions:

// Get the entity position realitive to the screen
EntityProjection getScreenProjection(Entity entity) {
    EntityProjection output;
    output.set(
        getpercent(entity.position.x, game.width, 2) - 1,
        getpercent(entity.position.y, game.height, 2) - 1,
        getpercent(entity.radius, game.width + game.height, 4)
    );
    return output;
};

// Draw a polygon to display
void DrawPolygon(double x, double y, double radius, int detail) {
    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < detail; ii++) {
        double theta = 2.0 * 3.1415926 * double(ii) / double(detail);
        glVertex2f(radius * cosf(theta) + x, radius * sinf(theta) + y);
    };
    glEnd();
};

// Main drawing/Physics loop
void gameLoop () {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);
    if (game.entityCount < game.entityLimit) game.createEntity();
    for (std::map<int, Entity>::iterator key = game.world.begin(); key != game.world.end(); key++) {
        Entity entity = key->second;

        // Entity collision
        for (std::map<int, Entity>::iterator iterator = game.world.begin(); iterator != game.world.end(); iterator++) {
            if (key->first == iterator->first) continue; // If they're the same shape
            Entity target = iterator->second;
            if (getCollision(entity, target) == false) continue; // Make sure they are colliding
            // Collision math
            double hitAngle = atan2(target.position.y - entity.position.y, target.position.x - entity.position.x);
            double pushVelocity[2] = { cos(hitAngle), sin(hitAngle) };
            entity.velocity.x -= pushVelocity[0] * game.collisionStrength;
            entity.velocity.y -= pushVelocity[1] * game.collisionStrength;
            target.velocity.x += pushVelocity[0] * game.collisionStrength;
            target.velocity.y += pushVelocity[1] * game.collisionStrength;

            game.world[iterator->first] = target;
        };
        // Out of bounds stuff
        if (entity.position.x > game.width || entity.position.x < 0) entity.velocity.x = -entity.velocity.x;
        if (entity.position.y > game.height || entity.position.y < 0) entity.velocity.y = -entity.velocity.y;

        // Velocity math
        entity.position.x += entity.velocity.x;
        entity.position.y += entity.velocity.y;

        game.world[key->first] = entity;
        EntityProjection projection = getScreenProjection(entity);
        // Drawing functions
        DrawPolygon(
            projection.position.x, // X
            projection.position.y, // Y
            projection.radius, // Size
            25 // Detail
        );
    };
    glFlush();
};

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(game.width, game.height);
    //glutInitWindowPosition(50, 50);
    glutCreateWindow("C++ Physics simulation by Sopur");
    glutDisplayFunc(gameLoop);
    while(true) {
        glutMainLoopEvent();
        gameLoop();
    };
    return 0;
};