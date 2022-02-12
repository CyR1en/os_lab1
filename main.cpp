#include <iostream>
#include <sys/utsname.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <list>
#include <string>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>

/**
 * Override << operator for struct uts
 */
std::ostream &operator<<(std::ostream &out, const struct utsname &uts) {
    out << "System Name  : " << uts.sysname << std::endl;
    out << "Node name    : " << uts.nodename << std::endl;
    out << "Release      : " << uts.release << std::endl;
    out << "Version      : " << uts.version << std::endl;
    out << "Machine      : " << uts.machine;
    return out;
}

/**
 * Split a string using a delimiter and push it back to the referenced vector of strings
 *
 * @param str The string to split
 * @param out Where each element of the string is going to be pushed back
 * @param delimiter The delimiter to use
 */
void tokenize(const std::string &str, std::vector<std::string> &out, const char *delimiter) {
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delimiter, end)) != std::string::npos) {
        end = str.find(delimiter, start);
        out.push_back(str.substr(start, end - start));
    }
}

/**
 * A function that parses each lines of the file into a vector. Each line is also parsed into
 * a vector. Essentially this parses a line into a vector (delimited by a space) and that vector
 * is pushed back to another vector that holds every line.
 *
 * This function also allows me to pass in a list of labels to only grab specific lines that starts
 * with those given labels. If provided with no labels, this function will simply grab every line.
 *
 * @param procPath
 * @param labels
 * @return
 */
std::vector<std::vector<std::string>> get_proc(const char *procPath, std::list<const char *> labels = {}) {
    std::ifstream is(procPath);
    std::string l;
    std::vector<std::vector<std::string>> allLines;
    while (std::getline(is, l)) {
        std::vector<std::string> lineVec;
        tokenize(l, lineVec, " ");
        if (labels.empty())
            allLines.push_back(lineVec);
        else if (std::find(labels.begin(), labels.end(), lineVec.at(0)) != labels.end())
            allLines.push_back(lineVec);
        lineVec.clear();
    }
    return allLines;
}

/**
 * Display a formatted time_t
 *
 * @param t time_t to format
 * @param label Label for the format
 * @param fmt format to display time_t with
 */
void display_time(time_t t, const char *label, const char *fmt) {
    auto tm = *std::localtime(&t);
    std::cout << label << std::put_time(&tm, fmt) << std::endl;
}

// Macros for constants
#define DAY_SEC 86400
#define HOUR_SEC 3600
#define MIN_SEC 60
// Ternary operator macro to fill in a 0.
#define FILL(x) ((x) > 9 ? "" : "0")

/**
 * Provided the amount of seconds, calculate how many days, hour, min, and sec. Then display
 * the data as dd:hh:mm:ss
 *
 * @param label Labor for the std::cout
 * @param elapsed_sec Elapsed seconds
 */
void display_formatted_elapsed(const char *label, u_long elapsed_sec) {
    u_int day = elapsed_sec / DAY_SEC;
    u_int hour = (elapsed_sec % DAY_SEC) / HOUR_SEC;
    u_int min = (elapsed_sec % HOUR_SEC) / MIN_SEC;
    u_int sec = elapsed_sec % MIN_SEC;
    std::cout << label << FILL(day) << day << ":" << FILL(hour) << hour << ":"
              << FILL(min) << min << ":" << FILL(sec) << sec << std::endl;
}


// Get boot time from /proc/stat
void get_boot_time() {
    auto bTime = get_proc("/proc/stat", {"btime"});
    auto t = (time_t) std::stol(bTime.at(0).at(1));
    display_time(t, "Boot time    : ", "%Y-%m-%d %H:%M:%S");
}

/**
 * Get amount of time in user mode and system mode from /proc/stat
 * Since the data is in USER_HRZ, I converted it by dividing with _SC_CLK_TCK
 */
void get_cpu_time() {
    auto bTime = get_proc("/proc/stat", {"cpu"});
    auto t = (time_t) std::stol(bTime.at(0).at(1)) / sysconf(_SC_CLK_TCK);
    display_formatted_elapsed("User-mode    : ", t);
    t = (time_t) std::stol(bTime.at(0).at(3)) / sysconf(_SC_CLK_TCK);
    display_formatted_elapsed("System-mode  : ", t);
}

// Get uptime and idle time from /proc/uptime
void get_uptime() {
    auto uptimeVec = get_proc("/proc/uptime");
    auto uptime = (time_t) std::stol(uptimeVec.at(0).at(0));
    auto idle_time = (time_t) std::stol(uptimeVec.at(0).at(1));
    display_formatted_elapsed("Uptime       : ", uptime);
    display_formatted_elapsed("Idle-time    : ", idle_time);
}

// Get total memory and available memory from /proc/meminfo
void get_mem_info() {
    auto  memInfo = get_proc("/proc/meminfo",{"MemTotal:", "MemAvailable:"});
    auto total = memInfo.at(0).at(1);
    std::cout << "MemTotal     : " << total << " kB" << std::endl;
    auto avail = memInfo.at(1).at(1);
    std::cout << "MemAvailable : " << total << " kB";
}

// Main func
int main() {
    utsname uts{};
    uname(&uts);
    std::cout << uts << std::endl;
    get_boot_time();
    get_uptime();
    get_cpu_time();
    get_mem_info();
    return 0;
}
