#pragma once

int main(int argc, char** argv) {
    nk::Logger::Init();
    auto app = nk::CreateApplication();
    app->Run();
    delete app;
}
