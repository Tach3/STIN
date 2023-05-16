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
          "accounts": [
            {
              "Currency": "USD",
              "account_number": 1776,
              "funds": 20
            },
            {
              "Currency": "EUR",
              "account_number": 1234,
              "funds": 100
            }
          ]
        },
        {
          "email": "peterspurnysemestralka@gmail.com",
          "password": "heslo123",
          "accounts": [
            {
              "Currency": "USD",
              "account_number": 885,
              "funds": 44
            },
            {
              "Currency": "EUR",
              "account_number": 567,
              "funds": 200
            }
          ]
        }
    ])"_json;

            // Test existing user
            string email = "peter.spurny@tul.cz";
            json user_info = get_user_info(email, data);
            Assert::AreEqual(user_info["account_1"]["account_number"].get<int>(), 1776);
            Assert::AreEqual(user_info["account_1"]["funds"].get<int>(), 20);
            Assert::AreEqual(user_info["account_1"]["Currency"].get<string>(), string("USD"));
            Assert::AreEqual(user_info["account_2"]["account_number"].get<int>(), 1234);
            Assert::AreEqual(user_info["account_2"]["funds"].get<int>(), 100);
            Assert::AreEqual(user_info["account_2"]["Currency"].get<string>(), string("EUR"));

            // Test another existing user
            email = "peterspurnysemestralka@gmail.com";
            user_info = get_user_info(email, data);
            Assert::AreEqual(user_info["account_1"]["account_number"].get<int>(), 885);
            Assert::AreEqual(user_info["account_1"]["funds"].get<int>(), 44);
            Assert::AreEqual(user_info["account_1"]["Currency"].get<string>(), string("USD"));
            Assert::AreEqual(user_info["account_2"]["account_number"].get<int>(), 567);
            Assert::AreEqual(user_info["account_2"]["funds"].get<int>(), 200);
            Assert::AreEqual(user_info["account_2"]["Currency"].get<string>(), string("EUR"));

            // Test non-existent email
            email = "nonexistentemail@test.com";
            user_info = get_user_info(email, data);
            Assert::IsTrue(user_info.empty());
        }

        TEST_METHOD(TestFindCurrency) {
            // Create some Currency objects to populate the vector
            Currency c1("USD", 1.0, 100);
            Currency c2("EUR", 1.2, 50);
            Currency c3("JPY", 0.009, 200);

            // Create a vector and add the currencies to it
            std::vector<Currency*> kurz;
            kurz.push_back(&c1);
            kurz.push_back(&c2);
            kurz.push_back(&c3);

            // Test finding an existing currency
            double rate = findCurrency("EUR", kurz);
            Assert::AreEqual(rate, 1.2);

            // Test finding a non-existing currency
            rate = findCurrency("GBP", kurz);
            Assert::AreEqual(rate, 0.0);
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
            std::ifstream infile(CNBTXT);
            Assert::AreEqual(infile.good(), true);
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

        TEST_METHOD(TestInsertCode) {
            // Prepare test data
            json data = R"([
        {
            "email": "john.doe@example.com",
            "password": "secret123",
            "funds": 100
        },
        {
            "email": "jane.doe@example.com",
            "password": "qwerty",
            "funds": 50
        }
    ])"_json;

            // Call function with test data
            int code = 5678;
            string email = "john.doe@example.com";
            insertCode(email, data, code);

            // Read the contents of the file
            ifstream i("users.json");
            json file_data;
            i >> file_data;

            // Check if the contents of the file are as expected
            json expected_data = R"([
        {
            "email": "john.doe@example.com",
            "password": "secret123",
            "funds": 100,
            "code": "5678"
        },
        {
            "email": "jane.doe@example.com",
            "password": "qwerty",
            "funds": 50
        }
    ])"_json;
            Assert::AreEqual(expected_data.dump(), file_data.dump());
        }

        TEST_METHOD(TestFindRate) {
            // Set up the test data
            vector<Currency*> kurz = {
                new Currency("USD", 1.0, 100),
                new Currency("EUR", 0.82, 50),
                new Currency("GBP", 0.72, 200),
                new Currency("JPY", 109.52, 1000)
            };
            string currencyTo = "EUR";
            string currencyFrom = "USD";
            double expectedRate = 0.82 / 1.0;

            // Call the function and check the result
            double actualRate = findRate(kurz, currencyTo, currencyFrom);
            Assert::AreEqual(actualRate,expectedRate);

            // Clean up the memory used by the test data
            for (auto currency : kurz) {
                delete currency;
            }
        }

        TEST_METHOD(TestFindCZRate)
        {
            // Create some currencies to test with
            Currency usd("USD", 21, 1);
            Currency eur("EUR", 23, 1);

            // Create a vector of currencies
            vector<Currency*> rate_v{ &usd, &eur};

            // Test CZK to EUR conversion
            string czkStr = "CZK";
            string eurStr = "EUR";
            string usdStr = "USD";
            double rate = findCZRate(rate_v, eurStr, czkStr);
            Assert::AreEqual(rate, 23.0000);
            rate = findCZRate(rate_v, czkStr, usdStr);
            Assert::AreEqual(rate, 1.0000/21.0000);
        }
        TEST_METHOD(ReturnsUserTransactions)
        {
            // Sample input data
            std::string email = "john.doe@example.com";

            json transactions = R"(
                {
                    "users": [
                        {
                            "email": "john.doe@example.com",
                            "accounts": [
                                {
                                    "account_number": "1234567890",
                                    "Currency": "USD",
                                    "transactions": [
                                        {
                                            "account": "9876543210",
                                            "date": "2023-05-01",
                                            "amount": 100.0
                                        },
                                        {
                                            "account": "9876543210",
                                            "date": "2023-05-02",
                                            "amount": -50.0
                                        },
                                        {
                                            "account": "9876543210",
                                            "date": "2023-05-03",
                                            "amount": 200.0
                                        }
                                    ]
                                }
                            ]
                        }
                    ]
                })"_json;

            // Call the function
            json result = get_user_trans(email, transactions);

            // Assert the expected response
            Assert::AreEqual(email, result["email"].get<std::string>());

            json accounts = result["accounts"];

            json account = accounts[0];
            Assert::AreEqual(string("1234567890"), account["account_number"].get<std::string>());
            Assert::AreEqual(string("USD"), account["Currency"].get<std::string>());       
        }

        TEST_METHOD(ReturnsUserAccount)
        {
            // Sample input data
            std::string email = "john.doe@example.com";
            std::string accountNumber = "1234567890";

            json data = R"(
                [
                    {
                        "email": "john.doe@example.com",
                        "accounts": [
                            {
                                "account_number": "9876543210",
                                "balance": 1000.0
                            },
                            {
                                "account_number": "1234567890",
                                "balance": 500.0
                            }
                        ]
                    },
                    {
                        "email": "jane.doe@example.com",
                        "accounts": [
                            {
                                "account_number": "5432109876",
                                "balance": 2000.0
                            }
                        ]
                    }
                ]
            )"_json;

            // Call the function
            json result = get_user_account(email, data, accountNumber);

            // Assert the expected account object
            Assert::IsTrue(result.is_object());
            Assert::AreEqual(accountNumber, result["account_number"].get<std::string>());
            Assert::AreEqual(500.0, result["balance"].get<double>());
        }

        TEST_METHOD(FillsKurzVectorFromFile)
        {

            std::vector<Currency*> kurz;
            fillKurz(kurz);

            Assert::AreEqual(size_t(31), kurz.size());
        }

        TEST_METHOD(TestSwitchAccountVariables)
        {
            // Arrange
            json transfer = R"({
                "accountFrom": "22222222",
                "accountNumber": "54806491",
                "amount": "500",
                "currency": "EUR"
            })"_json;

            // Act
            switchAccountVariables(transfer);

            // Assert
            Assert::AreEqual("54806491", transfer["accountFrom"].get<std::string>().c_str());
            Assert::AreEqual("22222222", transfer["accountNumber"].get<std::string>().c_str());
            Assert::AreEqual(to_string(stod("-500")), transfer["amount"].get<std::string>());
        }

        TEST_METHOD(TestGetUserCurrencies_UserFound)
        {
            // Arrange
            json data = R"([
                {
                    "email": "peter.spurny@tul.cz",
                    "code": "CZK",
                    "currencies": ["CZK", "EUR", "USD"]
                },
                {
                    "email": "john.doe@example.com",
                    "code": "USD",
                    "currencies": ["USD", "EUR"]
                }
            ])"_json;

            std::string email = "peter.spurny@tul.cz";
            std::string code = "CZK";

            // Act
            json result = getUserCurrencies(email, data, code);

            // Assert
            Assert::AreEqual("CZK", result[0].get<std::string>().c_str());
            Assert::AreEqual("EUR", result[1].get<std::string>().c_str());
            Assert::AreEqual("USD", result[2].get<std::string>().c_str());
        }

	};
}
