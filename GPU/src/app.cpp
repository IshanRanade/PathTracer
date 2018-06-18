#include "app.h"
#include "la.h"


// Start the program
int main(int argc, char **argv) {
    glm::vec3 x;    
    App app = App(argc, argv);
    return app.startApp();
}
