#include <nk.h>

class Game : public nk::Application {
public:
    Game() {}
    ~Game() {}
};

nk::Application* nk::CreateApplication() { return new Game(); }
