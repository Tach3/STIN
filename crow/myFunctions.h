#pragma once
#include "json.hpp"
#include <curl/curl.h>
#include <fstream>

using namespace std;
using json = nlohmann::json;

#define MAILFROM "peterspurnysemestralka@gmail.com"
#define SMTP "smtp.gmail.com:587"
#define USERSJ "users.json"
#define CNB "https://www.cnb.cz/cs/financni_trhy/devizovy_trh/kurzy_devizoveho_trhu/denni_kurz.txt"
#define DATAJ "data.json"
#define CNBTXT "cnb.txt"
#define TRANSJ "transactions.json"

class Currency {
private:
    string code;
    double rate;
    int amount;
public:
    Currency(string _code, double _rate, int _amount) {
        code = _code;
        rate = _rate;
        amount = _amount;
    }
    string getCode() { return code; }
    double getRate() { return rate; }
    void setRate(float kurz) { rate = kurz; }
    int getAmount() { return amount; }
    void setAmount(int mnozstvo) { amount = mnozstvo; }
};

void fillKurz(vector<Currency*>& kurz) {
    ifstream file(CNBTXT);
    string line;
    getline(file, line);
    getline(file, line);

    while (getline(file, line)) {
        replace(line.begin(), line.end(), ',', '.');
        double rate = stod(line.substr(line.find_last_of("|") + 1));
        line = line.substr(0, line.find_last_of("|"));
        string code = line.substr(line.find_last_of("|") + 1);
        line = line.substr(0, line.find_last_of("|"));
        int amount = stoi(line.substr(line.find_last_of("|") + 1));
        Currency* currency = new Currency(code, rate, amount);
        kurz.push_back(currency);
    }

}
void freeKurz(vector<Currency*>& kurz) {
    for (auto& currencyPtr : kurz) {
        delete currencyPtr;
    }
    kurz.clear();
}

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

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}


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

CURLcode downloadCNB() {
    CURL* curl_handle;
    const char* pagefilename = "cnb.txt";
    FILE* pagefile;

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* set URL to get here */
    curl_easy_setopt(curl_handle, CURLOPT_URL, CNB);

    /* Switch on full protocol/debug output while testing */
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

    /* disable progress meter, set to 0L to enable it */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    CURLcode res = CURLE_FTP_COULDNT_RETR_FILE;

    /* open the file */
    pagefile = fopen(pagefilename, "wb");
    if (pagefile) {

        /* write the page body to this file handle */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

        /* get it! */
        curl_easy_perform(curl_handle);

        /* close the header file */
        fclose(pagefile);
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

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

json get_user_trans(std::string email, json transactions) {
    json response;

    for (auto& user : transactions["users"]) {
        if (user["email"] == email) {
            // Found the user, return the top 4 transactions
            response["email"] = user["email"];
            response["transactions"] = json::array();

            int count = 0;
            for (auto& trans : user["transactions"]) {
                if (count >= 4) break;

                json transaction;
                transaction["from"] = trans.at("from");
                transaction["account"] = trans.at("account");
                transaction["date"] = trans.at("date");
                transaction["amount"] = trans.at("amount");
                response["transactions"].push_back(transaction);

                count++;
            }

            return response;
        }
    }

    // User not found, return an empty JSON object
    return response;
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