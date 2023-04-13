#include "crow_all.h"
#include <iostream>
#include "json.hpp"
#include <fstream>
using namespace std;
using json = nlohmann::json;

struct Credentials {
    string username;
    string password;
};

json get_user_info(const string& username) {
    ifstream json_file("data.json");
    json data;
    json_file >> data;

    // Search for the user with username "Tache"
    for (auto& user : data) {
        if (user["username"] == username) {
            // Found the user, print the account number and funds
            return json{ {"account_number", user["account_number"]},{"funds", user["funds"]} };
           //std::cout << "Account number: " << user["account_number"] << std::endl;
            //std::cout << "Funds: " << user["funds"] << std::endl;

        }
    }
  /*  for (const auto& user : data) {
        if (user["username"] == username) {
            // Return account number and funds as JSON object
            return json{ {"account_number", user["account_number"]},
                        {"funds", user["funds"]} };
        }
    }*/

    // If user is not found, return empty JSON object
    return json{};
}

bool verify_login(const string& username, const string& password) {
    return true;
}

int main()
{
    crow::SimpleApp app;

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([]() {
        auto page = crow::mustache::load_text("login.html");
        cout << "index" << endl;
        return page;
        });

    CROW_ROUTE(app, "/login")
        .methods("POST"_method)
        ([](const crow::request& req) {
        // Parse JSON data from request body
        json req_body = json::parse(req.body);

        // Get username and password from request body
        std::string username = req_body["username"].get<std::string>();
        std::string password = req_body["password"].get<std::string>();

        // Verify login credentials
        bool is_valid_login = verify_login(username, password);

        // Create JSON response data
        json response_data;
        if (is_valid_login) {
            response_data["success"] = true;
            response_data["message"] = "Login succeeded";
            response_data["user_info"] = get_user_info(username); // Custom function to get user info
        }
        else {
            response_data["success"] = false;
            response_data["message"] = "Invalid username or password";
        }

        // Return JSON response
        return crow::response{ response_data.dump() };
            });
    


    // Define a welcome page to redirect the user to after a successful login
    CROW_ROUTE(app, "/welcome")([]() {
        return "Welcome!";
        });
    

    app.port(18080).multithreaded().run();
}
