#include <iostream>
#include <vector>
#include <restclient-cpp/restclient.h>
#include <nlohmann/json.hpp>
#include <unistd.h>
#include <cstdlib>
#include "colors.h"
using json = nlohmann::json;
std::vector<std::string> split(std::string string, char enc) {
    std::string z;
    std::vector<std::string> z1;
    for (int i = 0; i < string.size(); ++i) {
        if (string[i] != enc) {
            z += string[i];
        } else {
            z1.push_back(z);
            z = "";
        }
    }
    if (!z.empty()) { // Add the last segment if non-empty
        z1.push_back(z);
    }
    return z1;
}
std::string encode(std::string url) {
    std::string enc;
    for (int i = 0; i < url.size(); i++) {
        if (url[i] == ' ') {
            enc = enc + "%20";
        } else {
            enc = enc + url[i];
        }
    }
    return enc;
}
void searchaur(std::string search="screen recorder") {
    RestClient::Response res = RestClient::get(encode("https://aur.archlinux.org/rpc/v5/search/" + search));
    json dataz = json::parse(res.body);
    for (const auto& result : dataz["results"]) {
        std::cout << HBLU << "aur" << reset << "/" << HRED << result["Name"].get<std::string>() << reset <<std::endl;
        std::cout << HYEL << "\t" << result["Description"] << reset << std::endl;
    }
}
void searchmain(std::string search="firefox") {
    RestClient::Response res = RestClient::get(encode("https://archlinux.org/packages/search/json/?q=" + search));
    json dataz = json::parse(res.body);
    for (const auto& result : dataz["results"]) {
        std::cout << HBLU << result["repo"].get<std::string>() << reset << "/" << HYEL << result["pkgname"].get<std::string>() << reset << std::endl;
        std::cout << HRED << "\t" <<  result["pkgdesc"] << reset << std::endl;
    }
}
int main(int argc, char* argv[]) {
    std::string man;
    for (int i = 1; i < argc; i++) {
        if (!man.empty()) {
            man += ' ';
        }
        man += argv[i];
    }
    std::vector<std::string> potato = split(man, ' ');
    for (int i = 0; i < potato.size(); ++i) {
        if (potato[i] == "-a" || potato[i] == "--aur") {
            searchaur(potato[i+1]);
        } else if (potato[i] == "-p" || potato[i] == "--packages") {
            searchmain(potato[i+1]);
        } else if (potato[i] == "-ia" || potato[i] == "--installaur") {
            std::vector<std::string> programs;
            for (int ii = i+1;ii < potato.size();ii++) {
                programs.push_back(potato[ii]);
            }
            for (const auto& program : programs) {
                std::cout << program << std::endl;
                // check if it exists
                // prepare for bad code
                RestClient::Response res = RestClient::get(encode("https://aur.archlinux.org/rpc/v5/search/" + program));
                json j = json::parse(res.body);
                std::cout << encode("https://aur.archlinux.org/rpc/v5/" + program) << std::endl;
                std::cout << j["resultcount"] << std::endl;
                for (const auto& response : j["results"]) {
                    if (response["Name"] == program) {
                        std::string a = "git clone https://aur.archlinux.org/" + program + ".git";
                        std::system(a.c_str());
                        chdir(program.c_str());
                        std::system("makepkg -si");
                    }
                }
            }
        } else if (potato[i] == "-ib" || potato[i] == "--installbase") {
            std::string cmd = "sudo pacman -S " + potato[i+1];
            std::system(cmd.c_str());
        } else if (potato[i] == "-h" || potato[i] == "--help") {
            std::cout << "WIP" << std::endl;
            std::cout << HRED << "-i[b (base)][a (aur)]: install packages" << std::endl;
            std::cout << HBLU <<"-[a (search aur)][p (search base)]" << std::endl;
        }
    }
}
