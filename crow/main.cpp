#define _CRT_SECURE_NO_WARNINGS
#include "crow_all.h"
#include <iostream>
#include "json.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "myFunctions.h"
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


using namespace std;
using json = nlohmann::json;

struct Credentials {
   const string username;
   const string password;
};

enum class SameSitePolicy
{
    Strict,
    Lax,
    None
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
        
        vector<Currency*> kurz;
        fillKurz(kurz);
        crow::App<crow::CookieParser, crow::CORSHandler> app;
        //CORS handler
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
            .prefix("/transactions")
            .origin("http://localhost:8000")
            .allow_credentials()
            .headers("*", "content-type")
            .prefix("/check")
            .origin("http://localhost:8000")
            .allow_credentials()
            .headers("*", "content-type")
            .prefix("/transfer")
            .origin("http://localhost:8000")
            .allow_credentials()
            .headers("*", "content-type")
            .prefix("/cors")
            .origin("*")
            .prefix("/nocors")
            .ignore();

        CROW_ROUTE(app, "/check").methods("POST"_method)
            ([&](const crow::request& req) {
            
            auto& ctx = app.get_context<crow::CookieParser>(req);
            if (!ctx.get_cookie("session").empty()) {
                
                return crow::response(200);
            }
            else {
                return crow::response(404);
            }
            });
        
        CROW_ROUTE(app, "/transfer").methods("POST"_method)
            ([&](const crow::request& req) {
            //{"accountFrom": "22222222","accountNumber":"45116119","amount":"456","currency":"CZK"}
            auto& ctx = app.get_context<crow::CookieParser>(req);
            json transfer = json::parse(req.body);
            string email = ctx.get_cookie("session");
            json data = parseJson(DATAJ);
            json customer = get_user_account(email, data, transfer["accountFrom"]); //{"account_1":{"Currency":"CZK","account_number":"22222222","funds":"1989"},"account_2":{"Currency":"USD","account_number":"22222223","funds":"1500"}}
            string amount = transfer["amount"];
            string funds = customer["funds"];
           if (transfer["currency"] == customer["Currency"]) {//currency sedi
                if (stoi(amount) < stoi(funds)) {
                    //todo edit dataj, edit transactions
                    string transfer_amount = transfer["amount"];
                    string customer_account = customer["account_number"];
                    updateUserData(email, customer_account, transfer_amount);
                    updateTransactions(email, transfer);
                    return crow::response(200);
                }
                else {
                    return crow::response(404);
                }
            }
            else {//currency nesedi
                string currencyTo = transfer["currency"];
                string currencyFrom = customer["Currency"];
                double c_rate;
                if (currencyTo == "CZK" || currencyFrom == "CZK") {
                    c_rate = findCZRate(kurz, currencyTo, currencyFrom);
                }
                else {
                    c_rate = findRate(kurz, currencyTo, currencyFrom);
                }
                double transferAmount = stoi(amount) * c_rate;
                  if (transferAmount < stoi(funds)) {
                    string transfer_amount = to_string(transferAmount);
                    string customer_account = customer["account_number"];
                    transfer["amount"] = transfer_amount;
                    updateUserData(email, customer_account, transfer_amount);
                    updateTransactions(email, transfer);
                       return crow::response(200);
                   }
                   else {
                       return crow::response(404);
                   }
               }
                
                return crow::response(200);
          
            });
        

        CROW_ROUTE(app, "/login")
            .methods("POST"_method)
            ([&](const crow::request& req) {

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
            json response = getUserCurrencies(mail, data, code);
            if (!response.empty()) {
                
                auto& ctx = app.get_context<crow::CookieParser>(req);

                // set a cookie
                ctx.set_cookie("session", mail)
                    .path("/")
                    .secure()
                    .same_site(crow::CookieParser::Cookie::SameSitePolicy::None);
                    
            }
            else {
                return crow::response(404);
            }
            crow::response res{ response.dump() };
            return res;
            });

        CROW_ROUTE(app, "/dashboard").methods("POST"_method)
            ([&](const crow::request& req) {
            // Parse cookies using the CookieParser middleware
            auto& ctx = app.get_context<crow::CookieParser>(req);
            // Check if the "key" cookie exists
            if (!ctx.get_cookie("session").empty()) {
                string email = ctx.get_cookie("session");
                json data = parseJson(DATAJ);
                json response_data;
                response_data["user_info"] = get_user_info(email, data);
                crow::response res{ response_data.dump() };
                return res;
            }
            else {
                return crow::response(404);
            }
            });
        CROW_ROUTE(app, "/transactions").methods("POST"_method)
            ([&](const crow::request& req) {
            // Parse cookies using the CookieParser middleware
            auto& ctx = app.get_context<crow::CookieParser>(req);
            // Check if the "key" cookie exists
            if (!ctx.get_cookie("session").empty()) {
                string email = ctx.get_cookie("session");
                json transactions = parseJson(TRANSJ);
                json response_tran = get_user_trans(email, transactions);
                crow::response res{ response_tran.dump() };
                return res;
            }
            else {
                return crow::response(404);
            }
            });
        CROW_ROUTE(app, "/cronjob")([&]() {
            CURLcode response;
            response = downloadCNB();
            freeKurz(kurz);
            fillKurz(kurz);
            return crow::response(200);
            });

        char* port = std::getenv("PORT");
        uint16_t iPort = static_cast<uint16_t>(port != NULL ? stoi(port) : 18080);
        std::cout << "Port: " << iPort << endl;
        app.port(iPort).multithreaded().run();

}

