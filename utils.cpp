#include "utils.h"
#include <algorithm>
#include <cctype>
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include <sstream>
#include <random>

// Simple round function (key-mixed hash)
uint32_t feistelRound(uint32_t halfBlock, uint32_t roundKey) {
    return (halfBlock ^ roundKey) + ((halfBlock & 0x0F0F0F0F) << 2);
}

std::string feistelEncrypt(const std::string& plaintext, const std::string& key) {
    std::ostringstream encrypted;

    size_t paddedLen = ((plaintext.size() + 7) / 8) * 8; // pad to 8 bytes
    std::string padded = plaintext;
    padded.resize(paddedLen, '\0'); // null padding

    for (size_t i = 0; i < padded.size(); i += 8) {
        uint32_t left = *reinterpret_cast<const uint32_t*>(&padded[i]);
        uint32_t right = *reinterpret_cast<const uint32_t*>(&padded[i + 4]);

        for (int round = 0; round < 8; ++round) {
            uint32_t temp = right;
            right = left ^ feistelRound(right, key[round % key.size()]);
            left = temp;
        }

        encrypted.write(reinterpret_cast<const char*>(&left), 4);
        encrypted.write(reinterpret_cast<const char*>(&right), 4);
    }

    return encrypted.str();
}

std::string feistelDecrypt(const std::string& ciphertext, const std::string& key) {
    std::ostringstream decrypted;

    for (size_t i = 0; i < ciphertext.size(); i += 8) {
        uint32_t left = *reinterpret_cast<const uint32_t*>(&ciphertext[i]);
        uint32_t right = *reinterpret_cast<const uint32_t*>(&ciphertext[i + 4]);

        for (int round = 7; round >= 0; --round) {
            uint32_t temp = left;
            left = right ^ feistelRound(left, key[round % key.size()]);
            right = temp;
        }

        decrypted.write(reinterpret_cast<const char*>(&left), 4);
        decrypted.write(reinterpret_cast<const char*>(&right), 4);
    }

    std::string result = decrypted.str();
    // Remove trailing null padding
    while (!result.empty() && result.back() == '\0') result.pop_back();
    return result;
}

bool isBlank(const std::string& str) {
    return std::ranges::all_of(str, [](unsigned char c) {
        return std::isspace(c);
    });
}

std::string getMaskedInput(const std::string& prompt) {
    std::string password;
    std::cout << prompt << std::flush;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') {  // carriage return
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
            std::cout << "\b \b";
        } else if (std::isprint(ch)) {
            password += ch;
            std::cout << '*';
        }
    }
#else
    // Unix/macOS version that shows '*' as you type
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n') {
        if (ch == 127 || ch == '\b') { // handle backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else if (std::isprint(ch)) {
            password += ch;
            std::cout << '*';
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    std::cout << std::endl;
    return password;
}

std::string generatePassword(int length, bool useSymbols, bool useNumbers, bool useUppercase) {
    const std::string lower = "abcdefghijklmnopqrstuvwxyz";
    const std::string upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string numbers = "0123456789";
    const std::string symbols = "!@#$%^&*()_+-=[]{}|;:,.<>?";

    std::string charset;
    if (useUppercase) charset += upper;
    if (useNumbers) charset += numbers;
    if (useSymbols) charset += symbols;

    if (charset.empty()) return "";
    charset += lower;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> dist(0, charset.size() - 1);

    std::string password;
    for (int i = 0; i < length; ++i) {
        password += charset[dist(rng)];
    }

    return password;
}


void copyToClipboard(const std::string& text) {
#ifdef __APPLE__
    FILE* pipe = popen("pbcopy", "w");
    if (pipe) {
        fwrite(text.c_str(), sizeof(char), text.length(), pipe);
        pclose(pipe);
    }
#endif
    // TODO: Add support for Windows or Linux (xclip / clip)
}

void clearClipboard() {
#ifdef __APPLE__
    FILE* pipe = popen("pbcopy", "w");
    if (pipe) {
        fputs(" ", pipe);  // Clear clipboard with a blank space
        pclose(pipe);
    }
#endif
}
