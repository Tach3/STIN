#pragma once
#include "json.hpp"
#include <curl/curl.h>
#include <fstream>

using namespace std;
using json = nlohmann::json;

#define MAILFROM "peter.spurny@tul.cz"
#define SMTP "smtp.tul.cz:587"
#define USERSJ "users.json"

json parseJson(std::string file) {
    ifstream json_file(file);
    json data;
    json_file >> data;
    return data;
}

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

CURLcode sendEmail(const std::string& recipient, int& code, std::string username, std::string password)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        fprintf(stderr, "curl_easy_init failed\n");

    }
    
    curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
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
    return res;
}

bool isPokus(int& param) {
	if (param > 3) {
		return true;
	}
	else {
		return false;
	}
}

json get_user_info(const string& email, json& data) {

    // Search for the user with email
    for (auto& user : data) {
        if (user["email"] == email) {
            // Found the user, print the account number and funds
            return json{ {"account_number", user["account_number"]},{"funds", user["funds"]} };

        }
    }

    // If user is not found, return empty JSON object
    return json{};
}

bool verify_login(const string& email, const string& password, json& data) {
    for (auto& user : data) {
        if (user["email"] == email && user["password"] == password) {
            // Found the user, return true
            return true;

        }
    }
    return false;
}

void insertCode(const string& email, json& data, int& code) {
    for (auto& user : data) {
        if (user["email"] == email) {
            // Found the user, insert code
            user["code"] = to_string(code);
            break;
        }
    }
    ofstream o("users.json");
    o << data.dump() << endl;
}

bool verifyCode(const string& email, json& data, string& code) {
    for (auto& user : data) {
        if (user["email"] == email) {
            // Found the user, verify code
            if (user["code"] == code) {
                return true;
            }
            else {
                return false;
            }
        }
    }
    return false;
}

int generateRandomNumber() {
    srand(time(NULL));  // Seed the random number generator with the current time
    int random_number = rand() % 9000 + 1000;  // Generate a random number between 1000 and 9999
    return random_number;
}