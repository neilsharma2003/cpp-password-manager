#include <iostream>
#include "utils.h"
#include "CredentialManager.h"
#include <limits>
#include <thread>

int main() {
    CredentialManager manager;

    std::string masterPwd = getMaskedInput("Enter master password: ");

    if (!manager.load(masterPwd)) {
        std::cout << "Failed to load credentials. Exiting.\n";
        return 1;
    }

    while (true) {
        std::cout << "\n1. Add Credential\n2. List Credentials\n3. Delete Credential\n4. Save and Exit\n5. Copy a Password to Clipboard\n6. Generate New Password\nChoice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            std::string site, user, pass;

            while (true) {
                std::cout << "Website: ";
                std::getline(std::cin, site);
                if (!isBlank(site)) break;
                std::cout << "Website can't be blank.\n";
            }

            while (true) {
                std::cout << "Username: ";
                std::getline(std::cin, user);
                if (!isBlank(user)) break;
                std::cout << "Username can't be blank.\n";
            }

            while (true) {
                pass = getMaskedInput("Password: ");
                if (!isBlank(pass)) break;
                std::cout << "Password can't be blank.\n";
            }

            manager.add(site, user, pass);
        } else if (choice == 2) {
            std::string response;
            bool reveal = false;

            while (true) {
                std::cout << "Reveal passwords? (y/n): ";
                std::getline(std::cin, response);

                if (response == "y" || response == "Y") {
                    reveal = true;
                    break;
                } else if (isBlank(response) || response == "n" || response == "N") {
                    reveal = false;
                    break;
                } else {
                    std::cout << "Please enter 'y' or 'n'.\n";
                }
            }
            manager.list(reveal);
        } else if (choice == 3) {
            std::string site;
            std::cout << "Website to delete: "; std::getline(std::cin, site);
            manager.remove(site);
        } else if (choice == 4) {
            bool _isSaved = manager.save(masterPwd);
            std::cout << "Saved and exiting.\n";
            break;
        } else if (choice == 5) {
            std::string site;
            std::cout << "Website to copy password from: ";
            std::getline(std::cin, site);

            auto cred = manager.getCredential(site);

            if (cred) {
                copyToClipboard(cred->password);
                std::cout << "Password copied to clipboard. It will be cleared in 30 seconds.\n";

                std::thread([] {
                    std::this_thread::sleep_for(std::chrono::seconds(30));
                    clearClipboard();
                }).detach();
            } else {
                std::cout << "No credential found for that website.\n";
            }
        }
        else if (choice == 6) {
            int length;
            std::string input;
            bool useSymbols = false, useNumbers = false, useUppercase = false;

            std::cout << "Password length: ";
            std::cin >> length;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Include symbols? (y/n): ";
            std::getline(std::cin, input);
            useSymbols = (input == "y" || input == "Y");

            std::cout << "Include numbers? (y/n): ";
            std::getline(std::cin, input);
            useNumbers = (input == "y" || input == "Y");

            std::cout << "Include uppercase letters? (y/n): ";
            std::getline(std::cin, input);
            useUppercase = (input == "y" || input == "Y");

            std::string generated = generatePassword(length, useSymbols, useNumbers, useUppercase);

            if (generated.empty()) {
                std::cout << "Invalid options. Password not generated.\n";
            } else {
                std::cout << "Generated Password: " << generated << '\n';
                copyToClipboard(generated);
                std::cout << "Copied to clipboard. Will clear in 30 seconds.\n";

                std::thread([] {
                    std::this_thread::sleep_for(std::chrono::seconds(30));
                    clearClipboard();
                }).detach();
            }
        }
    }

    return 0;
}
