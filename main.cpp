#define _CRT_SECURE_NO_WARNINGS
#include "crow.h"
using namespace std;
//#include "crow_all.h"

int main()
{
    crow::SimpleApp app;

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")
        ([]() {
        return "<div><h1>Hello world</h1></div>";
            });
    char* port = getenv("PORT");
    uint16_t iPort = static_cast<uint16_t>(port != NULL ? stoi(port) : 18080);
    cout << "PORT = " << iPort << endl;
    app.port(iPort).multithreaded().run();

}
