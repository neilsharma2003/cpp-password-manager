#include <cassert>
#include <iostream>
#include "utils.h"
#include "CredentialManager.h"

void testFeistelRoundTrip() {
    std::string key = "testkey";
    std::string original = "my secret message";
    std::string encrypted = feistelEncrypt(original, key);
    std::string decrypted = feistelDecrypt(encrypted, key);
    assert(decrypted == original);
    std::cout << "[PASSED] Feistel encryption/decryption\n";
}

void testIsBlank() {
    assert(isBlank("   "));
    assert(isBlank("\t\n"));
    assert(!isBlank("abc"));
    std::cout << "[PASSED] isBlank()\n";
}

void testCredentialAddAndGet() {
    CredentialManager manager;
    manager.add("example.com", "user", "pw123");
    auto cred = manager.getCredential("example.com");
    assert(cred);
    assert(cred->username == "user");
    assert(cred->password == "pw123");
    std::cout << "[PASSED] Credential add/getCredential()\n";
}

void testSaveLoad() {
    CredentialManager manager;
    std::string master = "testmaster";
    manager.add("test.com", "user", "1234");
    manager.save(master);

    CredentialManager loaded;
    bool success = loaded.load(master);
    assert(success);
    auto cred = loaded.getCredential("test.com");
    assert(cred);
    assert(cred->username == "user");
    assert(cred->password == "1234");
    std::cout << "[PASSED] Save/load cycle\n";
}

void testPasswordLength() {
    std::string pw = generatePassword(16, true, true, true);
    assert(pw.length() == 16);
    std::cout << "[PASSED] Password length == 16\n";
}

void testPasswordIncludesSymbols() {
    std::string pw = generatePassword(32, true, false, false);
    std::string symbols = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    assert(std::any_of(pw.begin(), pw.end(), [&](char c) {
        return symbols.find(c) != std::string::npos;
    }));
    std::cout << "[PASSED] Password includes symbols\n";
}

void testPasswordIncludesNumbers() {
    std::string pw = generatePassword(32, false, true, false);
    assert(std::any_of(pw.begin(), pw.end(), ::isdigit));
    std::cout << "[PASSED] Password includes numbers\n";
}

void testPasswordIncludesUppercase() {
    std::string pw = generatePassword(32, false, false, true);
    assert(std::any_of(pw.begin(), pw.end(), ::isupper));
    std::cout << "[PASSED] Password includes uppercase\n";
}

void testPasswordEmptyWhenNoCharset() {
    std::string pw = generatePassword(16, false, false, false);
    assert(pw.empty());
    std::cout << "[PASSED] Password empty when charset is empty\n";
}

int main() {
    testFeistelRoundTrip();
    testIsBlank();
    testCredentialAddAndGet();
    testSaveLoad();
    testPasswordLength();
    testPasswordIncludesSymbols();
    testPasswordIncludesNumbers();
    testPasswordIncludesUppercase();
    testPasswordEmptyWhenNoCharset();
    std::cout << "\nAll tests passed!\n";
    return 0;
}
