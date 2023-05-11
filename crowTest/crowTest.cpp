#include "pch.h"
#include "CppUnitTest.h"
#include "../crow/myFunctions.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace crowTest
{
	TEST_CLASS(crowTest)
	{
	public:
		
        TEST_METHOD(IsPokusTest)
        {
            int param1 = 4;
            int param2 = 2;

            // Test isPokus function with parameter greater than 3
            Assert::IsTrue(isPokus(param1));

            // Test isPokus function with parameter less than or equal to 3
            Assert::IsFalse(isPokus(param2));
        }

        TEST_METHOD(genRandomNumberTest)
        {
            int expectedLowerBound = 1000;
            int expectedUpperBound = 9999;

            // Act
            int result = generateRandomNumber();

            // Assert
            Assert::IsTrue(result >= expectedLowerBound && result <= expectedUpperBound);
            Assert::IsFalse(result <= expectedLowerBound && result >= expectedUpperBound);
        }

        TEST_METHOD(verify_loginTest)
        {
            nlohmann::json data = R"([
                            {
                              "email": "peter.spurny@tul.cz",
                              "password": "Heslo"
                            },
                            {
                              "email": "peterspurnysemestralka@gmail.com",
                              "password": "heslo123"
                            }
                          ])"_json;

            string email = "peter.spurny@tul.cz";
            string password = "Heslo";
            bool login_verified = verify_login(email, password, data);
            Assert::IsTrue(login_verified);

            email = "peterspurnysemestralka@gmail.com";
            password = "heslo123";
            login_verified = verify_login(email, password, data);
            Assert::IsTrue(login_verified);

            // Test invalid password
            email = "peter.spurny@tul.cz";
            password = "invalidpassword";
            login_verified = verify_login(email, password, data);
            Assert::IsFalse(login_verified);

            // Test non-existent email
            email = "nonexistentemail@test.com";
            password = "password";
            login_verified = verify_login(email, password, data);
            Assert::IsFalse(login_verified);
        }

        TEST_METHOD(TestGetUserInfo)
        {
            json data = R"([
                            {
                              "email": "peter.spurny@tul.cz",
                              "password": "Heslo",
                              "account_number": 1776,
                              "funds": 20
                            },
                            {
                              "email": "peterspurnysemestralka@gmail.com",
                              "password": "heslo123",
                              "account_number": 885,
                              "funds": 44
                            }
                          ])"_json;

            string email = "peter.spurny@tul.cz";
            json user_info = get_user_info(email, data);
            Assert::AreEqual(user_info["account_number"].get<int>(), 1776);
            Assert::AreEqual(user_info["funds"].get<int>(), 20);

            email = "peterspurnysemestralka@gmail.com";
            user_info = get_user_info(email, data);
            Assert::AreEqual(user_info["account_number"].get<int>(), 885);
            Assert::AreEqual(user_info["funds"].get<int>(), 44);

            // Test non-existent email
            email = "nonexistentemail@test.com";
            user_info = get_user_info(email, data);
            Assert::IsTrue(user_info.empty());
        }
        
        
	};
}
