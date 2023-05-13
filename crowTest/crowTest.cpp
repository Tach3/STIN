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

        TEST_METHOD(ReadsStringData)
        {
            // Set up test data
            std::string message = "This is a test message.";
            ReadData data(message.data());

            char buffer[1024];
            const size_t size = sizeof(buffer[0]);
            const size_t nitems = sizeof(buffer) / size;

            // Call the function being tested
            const size_t actual_len = read_function(buffer, size, nitems, &data);

            // Check the result
            Assert::AreEqual(message.length(), actual_len);
            Assert::IsTrue(std::memcmp(buffer, message.data(), message.length()) == 0);
        }
        
        TEST_METHOD(InsertsVerificationCode)
        {
            // Set up test data
            std::string email = "johndoe@example.com";
            int code = 12345;
            json data = R"([
                {
                    "name": "John Doe",
                    "email": "johndoe@example.com",
                    "code": "54321"
                },
                {
                    "name": "Jane Doe",
                    "email": "janedoe@example.com",
                    "code": "67890"
                }
            ])"_json;

            // Create a temporary file
            std::string filename = "test.json";
            std::ofstream f(filename);

            // Call the function being tested
            insertCode(email, data, code);

            // Write the updated data to the file
            f << data.dump();
            f.close();

            // Read the file back in and verify its contents
            bool result = verifyCode(email, data, to_string(code));

            Assert::IsTrue(result);

            // Clean up the temporary file
            std::remove(filename.c_str());
        }

        TEST_METHOD(VerificationCodeWrongEmail)
        {
            // Set up test data
            std::string email = "wrongEmail@example.com";
            int code = 12345;
            json data = R"([
                {
                    "name": "John Doe",
                    "email": "johndoe@example.com",
                    "code": "54321"
                },
                {
                    "name": "Jane Doe",
                    "email": "janedoe@example.com",
                    "code": "67890"
                }
            ])"_json;

            // Create a temporary file
            std::string filename = "test.json";
            std::ofstream f(filename);

            // Call the function being tested
            insertCode(email, data, code);

            // Write the updated data to the file
            f << data.dump();
            f.close();

            // Read the file back in and verify its contents
            bool result = verifyCode(email, data, to_string(code));

            Assert::IsFalse(result);

            // Clean up the temporary file
            std::remove(filename.c_str());
        }

        TEST_METHOD(VerifiesCorrectCode)
        {
            // Set up test data
            std::string email = "janedoe@example.com";
            std::string code = "12345";
            json data = R"([
                {
                    "name": "John Doe",
                    "email": "johndoe@example.com",
                    "code": "54321"
                },
                {
                    "name": "Jane Doe",
                    "email": "janedoe@example.com",
                    "code": "12345"
                }
            ])"_json;

            // Call the function being tested
            bool result = verifyCode(email, data, code);

            // Verify the result
            Assert::IsTrue(result);
        }

        TEST_METHOD(FailsVerificationWithIncorrectCode)
        {
            // Set up test data
            std::string email = "janedoe@example.com";
            std::string code = "54321";
            json data = R"([
                {
                    "name": "John Doe",
                    "email": "johndoe@example.com",
                    "code": "54321"
                },
                {
                    "name": "Jane Doe",
                    "email": "janedoe@example.com",
                    "code": "12345"
                }
            ])"_json;

            // Call the function being tested
            bool result = verifyCode(email, data, code);

            // Verify the result
            Assert::IsFalse(result);
        }

        TEST_METHOD(FailsVerificationWithNonexistentEmail)
        {
            // Set up test data
            std::string email = "nonexistent@example.com";
            std::string code = "12345";
            json data = R"([
                {
                    "name": "John Doe",
                    "email": "johndoe@example.com",
                    "code": "54321"
                },
                {
                    "name": "Jane Doe",
                    "email": "janedoe@example.com",
                    "code": "67890"
                }
            ])"_json;

            // Call the function being tested
            bool result = verifyCode(email, data, code);

            // Verify the result
            Assert::IsFalse(result);
        }
        
        TEST_METHOD(ParsesValidJsonFile)
        {
            // Set up test data
            std::string filename = "test.json";
            std::ofstream ofs(filename);
            ofs << R"(
            {
                "name": "John Doe",
                "email": "johndoe@example.com"
            }
            )";
            ofs.close();

            // Call the function being tested
            json result = parseJson(filename);

            // Verify the result
            Assert::AreEqual(std::string("John Doe"), result["name"].get<std::string>());
            Assert::AreEqual(std::string("johndoe@example.com"), result["email"].get<std::string>());

            // Clean up test data
            std::remove(filename.c_str());
        }

        TEST_METHOD(sendInvalidEmail) 
        {
            CURLcode response;
            int code;
            response = sendEmail("peter.spurny@outlook.com", code, "john.doe@example.com", "passwordForJohn");
            Assert::AreEqual(to_string(CURLE_LOGIN_DENIED), to_string(response));
        }

        TEST_METHOD(downloadCeskeKurzy)
        {
            CURLcode response;
            response = downloadCNB();
            Assert::AreEqual(to_string(CURLE_OK), to_string(response));
            remove(CNBTXT);
        }

        TEST_METHOD(TestWriteData)
        {
            // Create a temporary file for testing
            const char* filename = "testfile.txt";
            FILE* file = fopen(filename, "wb");
            Assert::IsNotNull(file, L"Failed to create a temporary file.");

            // Define test data
            const char* data = "Hello, World!";
            size_t dataSize = strlen(data);
            size_t elementSize = sizeof(char);
            size_t elementCount = dataSize / elementSize;

            // Call the function under test
            size_t written = write_data((void*)data, elementSize, elementCount, file);

            // Verify the result
            Assert::AreEqual(dataSize, written, L"Number of written elements does not match expected.");

            // Clean up
            fclose(file);
            remove(filename);
        }

        TEST_METHOD(CorrectConstruction)
        {
            Currency currency("USD", 1.0, 100);
            Assert::AreEqual(currency.getCode(), std::string("USD"));
            Assert::AreEqual(currency.getRate(), 1.0);
            Assert::AreEqual(currency.getAmount(), 100);
        }

        TEST_METHOD(FreeRate) {
            Currency* currency = new Currency("USD", 1.0, 100);
            vector<Currency*> rate;
            rate.push_back(currency);
            freeKurz(rate);
            Assert::IsTrue(rate.empty());
        }
        

	};
}
