#include <iostream>
#include <sys/utsname.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <list>
#include <string>
#include <ctime>
#include <iomanip>
#include <cmath>

std::ostream &operator <<(std::ostream &out, const struct utsname &uts) {
    out << "== uname ==" << std::endl;
    out << "System Name : " << uts.sysname << std::endl;
    out << "Node name   : " << uts.nodename << std::endl;
    out << "Release     : " << uts.release << std::endl;
    out << "Version     : " << uts.version << std::endl;
    out << "Machine     : " << uts.machine << std::endl;
    return out;
}

void tokenize(const std::string &str, std::vector<std::string> &out, const char * delimiter) {
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delimiter, end)) != std::string::npos) {
        end = str.find(delimiter, start);
        out.push_back(str.substr(start, end - start));
    }
}


std::vector<std::vector<std::string>> get_proc(const char * procPath, std::list<const char *> labels) {
    std::ifstream is(procPath);
    std::string l;
    std::vector<std::vector<std::string>> allLines;
    while(std::getline(is, l)) {
        std::vector<std::string> lineVec;
        tokenize(l, lineVec, " ");
        if(labels.empty()) {
            allLines.push_back(lineVec);
        } else {
            if (std::find(labels.begin(), labels.end(), lineVec.at(0)) != labels.end())
                allLines.push_back(lineVec);
        }
        lineVec.clear();
    }
    return allLines;
}

void display_time(time_t t, const char * label, const char * fmt) {
    auto tm = *std::localtime(&t);
    std::cout << label << std::put_time(&tm, fmt) << std::endl;
}

#define DAY_SEC 86400
#define HOUR_SEC 3600
#define MIN_SEC 60
#define FILL(x) ((x) > 9 ? "" : "0")
void display_formatted_elapsed(const char * label, u_long elapsed_sec) {
    u_int day = elapsed_sec / DAY_SEC;
    u_int hour = (elapsed_sec % DAY_SEC) / HOUR_SEC;
    u_int min = (elapsed_sec % HOUR_SEC) / MIN_SEC;
    u_int sec = elapsed_sec % MIN_SEC;
    std::cout << label << FILL(day) << day << ":" << FILL(hour) << hour << ":"
        << FILL(min) << min << ":" << FILL(sec) << sec << std::endl;
 }

void get_boot_time() {
    std::cout << "== /proc/stat/ btime ==" << std::endl;
    auto bTime = get_proc("/proc/stat", {"btime"});
    auto t = (time_t) std::stol(bTime.at(0).at(1));
    display_time(t, "Boot time: ", "%Y-%m-%d %H:%M:%S");
    std::cout << std::endl;
}

void get_uptime() {
    std::cout << "== /proc/uptime ==" << std::endl;
    auto uptimeVec = get_proc("/proc/uptime", {});
    auto uptime = (time_t) std::stol(uptimeVec.at(0).at(0));
    auto idle_time = (time_t) std::stol(uptimeVec.at(0).at(1));
    display_formatted_elapsed("Uptime: ", uptime);
    display_formatted_elapsed("Idle-time: ", idle_time);
    std::cout << std::endl;
}

void get_mem_info() {
    std::cout << "== /proc/meminfo ==" << std::endl;
    std::vector<std::vector<std::string>> memInfo = get_proc("/proc/meminfo",
                                                             {"MemTotal:", "MemAvailable:"});
    for (const auto &item : memInfo) {
        for (const auto &item2 : item) {
            std::cout << item2 << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    utsname uts{};
    uname(&uts);
    std::cout << uts << std::endl;
    get_boot_time();
    get_uptime();
    get_mem_info();
    return 0;
}
