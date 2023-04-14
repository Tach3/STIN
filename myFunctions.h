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