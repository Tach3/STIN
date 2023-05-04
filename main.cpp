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

#define FROM    "peter.spurny@tul.cz"
#define TO		"peter.spurny@outlook.com"
#define USERNAME "peter.spurny"
#define PASSWORD "Hurikanps99"
//#define MAILTO "peter.spurny@outlook.com"
#define MAILFROM "peter.spurny@tul.cz"
#define SMTP "smtp.tul.cz:587"


struct ReadData
{
    explicit ReadData(const char* str)
    {
        source = str;
        size = strlen(str);
    }

    const char* source;
    size_t size;
};

size_t read_function(char* buffer, size_t size, size_t nitems, ReadData* data)
{
    size_t len = size * nitems;
    if (len > data->size) { len = data->size; }
    memcpy(buffer, data->source, len);
    data->source += len;
    data->size -= len;
    return len;
}
void sendEmail(const std::string& recipient, int& code)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        fprintf(stderr, "curl_easy_init failed\n");

    }

    curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
    curl_easy_setopt(curl, CURLOPT_URL, SMTP);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, MAILFROM);

    struct curl_slist* rcpt = NULL;
    rcpt = curl_slist_append(rcpt, recipient.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, rcpt);
    string message = "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n"
        "To: " + recipient + "\r\n" 
        "From: " MAILFROM "\r\n" 
        "Subject: Verification Code\r\n"
        "\r\n"
        "Code: " + to_string(code) + "\r\n" 
        "This is a test email.\r\n";
    ReadData data(message.data());
    curl_easy_setopt(curl, CURLOPT_READDATA, &data);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_function);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    // If your server doesn't have a proper SSL certificate:
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        curl_easy_cleanup(curl);
    }
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
        string email = req_body["email"].get<std::string>();
        string password = req_body["password"].get<std::string>();
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
            //response_data["user_info"] = get_user_info(email, data); // Custom function to get user info
        }
        else {
            response_data["success"] = false;
            response_data["message"] = "Invalid email or password";
        }

        // Return JSON response

        crow::response res{ response_data.dump() };
        //res.set_header("Access-Control-Allow-Origin", "*"); // Set the header here
        return res;
            });
    


    // Define a welcome page to redirect the user to after a successful login
    CROW_ROUTE(app, "/welcome")([]() {
        int code = generateRandomNumber();
        sendEmail("peter.spurny@outlook.com", code);
        //sendPythonEmail("peter.spurny@outlook.com", to_string(generateRandomNumber()));
        return "Welcome!";
        });

    CROW_ROUTE(app, "/2fa").methods("POST"_method)
        ([](const crow::request& req) {
        int code = generateRandomNumber();
        json req_body = json::parse(req.body);
        string mail = req_body["email"].get<string>();
        sendEmail(mail, code);
        return "Welcome";
        });


    app.port(18080).multithreaded().run();
}
