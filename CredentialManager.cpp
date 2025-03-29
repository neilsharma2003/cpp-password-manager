#include "CredentialManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "utils.h"

bool CredentialManager::load(const std::string& masterPassword) {
    std::ifstream in("data.enc", std::ios::binary);
    if (!in) return true;

    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string encrypted = buffer.str();
    std::string decrypted = feistelDecrypt(encrypted, masterPassword);

    std::istringstream iss(decrypted);
    Credential credential;
    while (iss >> credential.website >> credential.username >> credential.password) {
        credentials.push_back(credential);
    }

    return true;
}

bool CredentialManager::save(const std::string& masterPassword) const {
    std::ostringstream oss;
    for (const Credential& credential : credentials) {
        oss << credential.website << ' ' << credential.username << ' ' << credential.password << '\n';
    }

    std::string encrypted = feistelEncrypt(oss.str(), masterPassword);
    std::ofstream out("data.enc", std::ios::binary);
    out << encrypted;
    return true;
}

void CredentialManager::add(const std::string& website, const std::string& username, const std::string& password) {
    credentials.push_back({ website, username, password });
}

void CredentialManager::list(bool revealPasswords = true) const {
    for (const auto& c : credentials) {
        std::cout << "Website: " << c.website << ", Username: " << c.username << ", Password: ";
        if (revealPasswords) {
            std::cout << c.password;
        } else {
            std::cout << std::string(c.password.length(), '*');
        }
        std::cout << '\n';
    }
}

void CredentialManager::remove(const std::string& website) {
    credentials.erase(std::ranges::remove_if(credentials,
                                             [&](const Credential& c) { return c.website == website; }).begin(),
        credentials.end());
}

std::shared_ptr<const Credential> CredentialManager::getCredential(const std::string& website) const {
    auto it = std::ranges::find_if(credentials,
                                   [&](const Credential& c) { return c.website == website; });

    if (it != credentials.end()) {
        return std::make_shared<const Credential>(*it); // shared copy
    }
    return nullptr;
}
