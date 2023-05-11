#define _CRT_SECURE_NO_WARNINGS
#include "crow_all.h"
#include <iostream>
#include "json.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "myFunctions.h"
#include <curl\curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


using namespace std;
using json = nlohmann::json;

struct Credentials {
   const string username;
   const string password;
};


int main()
{
        const Credentials credentials = []() -> Credentials {
            try {
                //boost::property_tree::ptree config;
                //boost::property_tree::ini_parser::read_ini("config.ini", config);

                // Get the username and password from the configuration file
                //const std::string username = config.get<std::string>("username");
                //const std::string password = config.get<std::string>("password");
                const char* user = std::getenv("USRNM");
                const char* pass = std::getenv("PSSWD");
                std::string username(user);
                std::string password(pass);
                return { username, password };
            }
            catch (std::exception& ex) {
                std::cerr << "Failed to read the env variable: " << ex.what() << std::endl;
                // Handle the exception here
                exit(1);
            }
        }();


        crow::App<crow::CookieParser, crow::CORSHandler> app;
        //CORS handler
        app.get_middleware<crow::CookieParser>();
        auto& cors = app.get_middleware<crow::CORSHandler>();

        cors
            .global()
            .methods("POST"_method, "GET"_method, "OPTIONS"_method)
            .prefix("/2faCode")
            .origin("http://localhost:8000")
            .allow_credentials()
            .headers("*", "content-type")
            .prefix("/dashboard")
            .origin("http://localhost:8000")
            .allow_credentials()
            .headers("*", "content-type")
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
            json data = parseJson(USERSJ);

            // Verify login credentials
            bool is_valid_login = verify_login(email, password, data);

            // Create JSON response data
            json response_data;
            if (is_valid_login) {
                response_data["success"] = true;
                response_data["message"] = "Login succeeded";
                //response_data["user_info"] = get_user_info(email, data); // Custom function to get user info
            }
            else {
                response_data["success"] = false;
                response_data["message"] = "Invalid email or password";
            }

            // Return JSON response

            crow::response res{ response_data.dump() };
            return res;
                });



        // Define a welcome page to redirect the user to after a successful login
        CROW_ROUTE(app, "/welcome")([&]() {
            int code = generateRandomNumber();
            CURLcode response;
            response = sendEmail("peter.spurny@outlook.com", code,credentials.username, credentials.password);
            //sendPythonEmail("peter.spurny@outlook.com", to_string(generateRandomNumber()));
            return "Welcome!";
            });

        CROW_ROUTE(app, "/2fa").methods("POST"_method)
            ([&](const crow::request& req) {
            int code = generateRandomNumber();
            json req_body = json::parse(req.body);
            std::string mail = req_body["email"].get<std::string>();
            CURLcode response;
            response = sendEmail(mail, code, credentials.username, credentials.password);
            json data = parseJson(USERSJ);
            insertCode(mail, data, code);
            return "2fa";
                });

        CROW_ROUTE(app, "/2faCode").methods("POST"_method)([&](const crow::request& req) {
            json req_body = json::parse(req.body);
            std::string mail = req_body["email"].get<std::string>();
            std::string code = req_body["code"].get<std::string>();
            json data = parseJson(USERSJ);
            bool isVerified = verifyCode(mail, data, code);
            json response;
            if (isVerified) {
                response["success"] = true;
                response["message"] = "Login succeeded";
                auto& ctx = app.get_context<crow::CookieParser>(req);

                // set a cookie
                ctx.set_cookie("session", "true");
            }
            else {
                response["success"] = false;
                response["message"] = "Invalid code";
            }
            crow::response res{ response.dump() };
            return res;
            });

        CROW_ROUTE(app, "/dashboard").methods("GET"_method)
            ([&](const crow::request& req) {
            // Parse cookies using the CookieParser middleware
            auto& ctx = app.get_context<crow::CookieParser>(req);
            // Check if the "key" cookie exists
            if (!ctx.get_cookie("session").empty()) {
                return crow::response(200);
            }
            else {
                return crow::response(302);
            }
                });


        app.port(18080).multithreaded().run();

        
}
