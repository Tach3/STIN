#include "crow_all.h"
#include <iostream>
#include "json.hpp"
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "myFunctions.h"
#include <curl\curl.h>


using namespace std;
using json = nlohmann::json;

struct Credentials {
    string email;
    string password;
};

int generateRandomNumber() {
    srand(time(NULL));  // Seed the random number generator with the current time
    int random_number = rand() % 9000 + 1000;  // Generate a random number between 1000 and 9999
    return random_number;
}




int main()
{
    crow::App<crow::CORSHandler> app;
    //CORS handler
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .methods("POST"_method, "GET"_method)
        .prefix("/cors")
        .origin("*")
        .prefix("/nocors")
        .ignore();



    CROW_ROUTE(app, "/login")
        .methods("POST"_method)
        ([](const crow::request& req) {
        
        // Parse JSON data from request body

        json req_body = json::parse(req.body);

        // Get email and password from request body
        std::string email = req_body["email"].get<std::string>();
        std::string password = req_body["password"].get<std::string>();
        ifstream json_file("data.json");
        json data;
        json_file >> data;

        // Verify login credentials
        bool is_valid_login = verify_login(email, password, data);

        // Create JSON response data
        json response_data;
        if (is_valid_login) {
            response_data["success"] = true;
            response_data["message"] = "Login succeeded";
            response_data["user_info"] = get_user_info(email, data); // Custom function to get user info
        }
        else {
            response_data["success"] = false;
            response_data["message"] = "Invalid email or password";
        }

        // Return JSON response

        crow::response res{ response_data.dump() };
        res.set_header("Access-Control-Allow-Origin", "*"); // Set the header here
        return res;
            });
    


    // Define a welcome page to redirect the user to after a successful login
    CROW_ROUTE(app, "/welcome")([]() {
        //sendEmail("hikawa.renko99@gmail.com");
        sendPythonEmail("peter.spurny@outlook.com", to_string(generateRandomNumber()));
        return "Welcome!";
        });


    app.port(18080).multithreaded().run();
}
