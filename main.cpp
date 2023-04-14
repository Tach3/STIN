#include "crow_all.h"
#include <iostream>
#include "json.hpp"
#include <fstream>
using namespace std;
using json = nlohmann::json;

struct Credentials {
    string email;
    string password;
};


json get_user_info(const string& email) {
    ifstream json_file("data.json");
    json data;
    json_file >> data;

    // Search for the user with email "peter.spurny@tul.cz"
    for (auto& user : data) {
        if (user["email"] == email) {
            // Found the user, print the account number and funds
            return json{ {"account_number", user["account_number"]},{"funds", user["funds"]} };

        }
    }

    // If user is not found, return empty JSON object
    return json{};
}

bool verify_login(const string& email, const string& password) {
    return true;
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

        // Verify login credentials
        bool is_valid_login = verify_login(email, password);

        // Create JSON response data
        json response_data;
        if (is_valid_login) {
            response_data["success"] = true;
            response_data["message"] = "Login succeeded";
            response_data["user_info"] = get_user_info(email); // Custom function to get user info
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
        return "Welcome!";
        });
    

    app.port(18080).multithreaded().run();
}
