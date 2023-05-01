#pragma once
#ifndef JSON_HPP
#define JSON_HPP

#include "../crow/json.hpp"

#endif /* JSON_HPP */
using namespace std;
using json = nlohmann::json;



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
            // Found the user, print the account number and funds
            return true;

        }
    }
    return false;
}

//ghetto solution
void sendPythonEmail(const std::string& recipient, const std::string& code)
{
    string py_code = "import smtplib\n\n"
        "TO = '" + recipient + "'\n"
        "MSG = '" + code + "'\n\n"
        "s = smtplib.SMTP('smtp.tul.cz',587)\n"
        "s.ehlo()\n"
        "s.starttls()\n"
        "s.login('login','passwd')\n"
        "try:\n"
        "    s.sendmail('peter.spurny@tul.cz', TO, MSG)\n"
        "except:\n"
        "    print ('failed')\n";

    // create a new file with the Python code
    ofstream file("send_email.py");
    if (file.is_open()) {
        file << py_code;
        file.close();
        cout << "File created successfully." << endl;
    }
    else {
        cout << "Unable to create file." << endl;
    }

    // execute the Python file using the system function
    int result = system("python send_email.py");
    if (result == 0) {
        cout << "Python script executed successfully." << endl;
    }
    else {
        cout << "Python script execution failed." << endl;
    }
    
    if (remove("send_email.py") != 0) {
        cout << "Unable to delete the file." << endl;
    }
    else {
        cout << "File deleted successfully." << endl;
    }
    
}

int generateRandomNumber() {
    srand(time(NULL));  // Seed the random number generator with the current time
    int random_number = rand() % 9000 + 1000;  // Generate a random number between 1000 and 9999
    return random_number;
}