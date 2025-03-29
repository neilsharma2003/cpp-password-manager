#pragma once
#include <string>

std::string feistelEncrypt(const std::string& plaintext, const std::string& key);
std::string feistelDecrypt(const std::string& ciphertext, const std::string& key);
bool isBlank(const std::string& str);
std::string getMaskedInput(const std::string& prompt);
std::string generatePassword(int length, bool useSymbols, bool useNumbers, bool useUppercase);
void copyToClipboard(const std::string& text);
void clearClipboard();
