#pragma once
#include <string>
#include <vector>

struct Credential {
    std::string website;
    std::string username;
    std::string password;
};

class CredentialManager {
    public:
    bool load(const std::string& masterPassword);
    bool save(const std::string& masterPassword) const;
    void add (const std::string& website, const std::string& username, const std::string& password);
    void list(bool revealPasswords) const;
    void remove(const std::string& website);
    std::shared_ptr<const Credential> getCredential(const std::string& website) const;
    private:
    std::vector<Credential> credentials;
};