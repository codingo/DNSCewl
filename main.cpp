#include <iostream>
#include <vector>
#include <fstream>
#include <cstddef> 
#include "includes/CLI11/CLI11.hpp"
#include "includes/spdlog/spdlog.h"
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <algorithm>

using namespace std;
std::string target;
std::string target_list_file;
std::string exclude_list_file;
std::string append_list_file;
std::string prepend_list_file;
bool verbose_flag;
bool subs_flag;
bool no_color;
int limit;
int level;
bool no_repeat_flag;

vector<string> target_list;
vector<string> append_list;
vector<string> prepend_list;
vector<string> exclude_list;
vector<string> results;

void initialize();
vector<string> read_file(string);
void error_fatal(string);

void start();
void process_append();
void process_prepend();
void print_domain(string);
bool have_repeats(string);
int count_char(string domain, char character);

void print_header()
{
	// courtesy of http://www.patorjk.com/software/taag/#p=display&f=Epic&t=FastSub
	std::string header = R"sep(

▓█████▄  ███▄    █   ██████  ▄████▄  ▓█████  █     █░ ██▓    
▒██▀ ██▌ ██ ▀█   █ ▒██    ▒ ▒██▀ ▀█  ▓█   ▀ ▓█░ █ ░█░▓██▒    
░██   █▌▓██  ▀█ ██▒░ ▓██▄   ▒▓█    ▄ ▒███   ▒█░ █ ░█ ▒██░    
░▓█▄   ▌▓██▒  ▐▌██▒  ▒   ██▒▒▓▓▄ ▄██▒▒▓█  ▄ ░█░ █ ░█ ▒██░    
░▒████▓ ▒██░   ▓██░▒██████▒▒▒ ▓███▀ ░░▒████▒░░██▒██▓ ░██████▒
 ▒▒▓  ▒ ░ ▒░   ▒ ▒ ▒ ▒▓▒ ▒ ░░ ░▒ ▒  ░░░ ▒░ ░░ ▓░▒ ▒  ░ ▒░▓  ░

Michael Skelton (@codingo)
Luke Stephens (@hakluke)
Sajeeb Lohani (sml555_)                                                        
	)sep";
	spdlog::info("{}\n", header);
}

int main(int argc, char **argv)
{
	print_header();

    CLI::App app{"When provided with a list of domain names, generate a wordlist of potential subdomains to be tested for."};
    app.add_option("-t,--target", target, "Specify a single target.");
    app.add_option("-l,--tL,--target-list", target_list_file, "Specify a list of targets.");
    app.add_option("-e,--eL,--exclude-list", exclude_list_file, "Specify a list of targets to exclude.");
    app.add_option("-a,--apend-list", append_list_file, "Specify a file of words to append to a host.");
    app.add_option("-p,--prepend-list", prepend_list_file, "Specify a file of words to prepend  to a host.");
    app.add_flag("-v,--verbose_flag", verbose_flag, "If set then verbose_flag output will be displayed in the terminal.");
    app.add_flag("-s,--subs", subs_flag, "If set then only subdomains will be generated.");
    app.add_flag("--no-color", no_color, "If set then any foreground or background colours will be stripped out.");
    app.add_option("--limit", limit, "Specify a fixed word limit to output.");
    app.add_option("--level", level, "Specify the level of results to output.");
    app.add_flag("--no-repeats", no_repeat_flag, "Prevent repeated structures such as one.one.com");

    CLI11_PARSE(app, argc, argv);
    initialize();
    start();
}

int count_char(string domain, char character)
{
    int count = 0;
    for (int i = 0; i < domain.length(); i++)
    {
        if (domain[i] == character)
        {
            count++;
        }
    }
    return count;
}

bool have_repeats(string domain)
{
    vector<string> words;
    std::stringstream ss(domain);
    std::string token;
    while (std::getline(ss, token, '.'))
    {
        words.push_back(token);
    }
    for (std::size_t i = 0; i < words.size(); ++i)
    {
        for (std::size_t j = i + 1; j < words.size(); ++j)
        {
            if (words[i] == words[j])
            {
                spdlog::debug("Repeats found in {}.", domain);
                return true;
            }
        }
    }
    return false;
}

void process_append()
{
    spdlog::debug("Processing append list.");
    for (auto target = target_list.begin(); target != target_list.end(); ++target)
    {
        string targ = *target;
        size_t location;
        if (count_char(targ, '.') >= 2)
        {
            location = targ.find_first_of(".");
        }
        else
        {
            location = targ.find_last_of(".");
        }
        for (auto append = append_list.begin(); append != append_list.end(); ++append)
        {
            if (!(std::find(exclude_list.begin(), exclude_list.end(), *target) != exclude_list.end()))
            {
                string appnd = *append;

                // First one
                char a[1024] = {};
                strncpy(a, targ.c_str(), location);
                strcat(a, "-");
                char temp[1024] = {};
                strncpy(temp, appnd.c_str(), appnd.length() - 1);
                strcat(a, temp);
                strcat(a, targ.substr(location, targ.length() - location).c_str());
                print_domain(string(a));

                // Second one
                if (level == 1)
                {
                    char b[1024] = {};
                    strncpy(b, targ.c_str(), location);
                    strcat(b, ".");
                    char temp1[1024] = {};
                    strncpy(temp1, appnd.c_str(), appnd.length() - 1);
                    strcat(b, temp1);
                    strcat(b, targ.substr(location, targ.length() - location).c_str());
                    print_domain(string(b));
                }
                // Third one
                char c[1024] = {};
                strncpy(c, targ.c_str(), location);
                char temp2[1024] = {};
                strncpy(temp2, appnd.c_str(), appnd.length() - 1);
                strcat(c, temp2);
                strcat(c, targ.substr(location, targ.length() - location).c_str());
                print_domain(string(c));
            }
        }
    }
}

void print_domain(string domain)
{
    if (subs_flag)
    {
        if (count_char(domain, '.') >= 2)
        {
            if (no_repeat_flag)
            {
                if (!(have_repeats(domain)))
                    cout << domain << endl;
            }
            else
            {
                cout << domain << endl;
            }

            results.push_back(domain);
        }
    }
    else
    {
        if (no_repeat_flag)
        {
            if (!(have_repeats(domain)))
                cout << domain << endl;
        }
        else
        {
            cout << domain << endl;
        }

        results.push_back(domain);
    }
}

void process_prepend()
{
    spdlog::debug("Processing prepend list.");
    for (auto target = target_list.begin(); target != target_list.end(); ++target)
    {
        for (auto prepend = prepend_list.begin(); prepend != prepend_list.end(); ++prepend)
        {
            if (!(std::find(exclude_list.begin(), exclude_list.end(), *target) != exclude_list.end()))
            {
                string prep = *prepend;
                string targ = *target;
                // First one
                char a[1024] = {};
                strncpy(a, prep.c_str(), prep.length() - 1);
                strcat(a, targ.c_str());

                print_domain(string(a));

                // Second one
                if (level == 1)
                {
                    char b[1024] = {};
                    strncpy(b, prep.c_str(), prep.length() - 1);
                    strcat(b, "-");
                    strcat(b, targ.c_str());

                    print_domain(string(b));
                }
                // Third one
                char c[1024] = {};
                strncpy(c, prep.c_str(), prep.length() - 1);
                strcat(c, ".");
                strcat(c, targ.c_str());

                print_domain(string(c));
            }
        }
    }
}

void start()
{
    if (append_list.size() > 0)
    {
        process_append();
    }
    else
    {
        process_prepend();
    }
}
void initialize()
{
    if (verbose_flag)
    {
        spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    }
    level = 1;
    if (target == "" && target_list_file == "")
    {
        error_fatal("At least single target needed to process");
    }
    if (append_list_file == "" && prepend_list_file == "")
    {
        error_fatal("File name of words to append or prepend to host is needed");
    }
    else if (append_list_file != "")
    {
        spdlog::debug("Reading append list.");
        append_list = read_file(append_list_file);
        if (append_list.size() == 0)
        {
            error_fatal("Please check " + append_list_file + ", no words read");
        }
    }
    else if (prepend_list_file != "")
    {
        spdlog::debug("Reading prepend list.");
        prepend_list = read_file(prepend_list_file);
        if (prepend_list.size() == 0)
        {
            error_fatal("Please check " + prepend_list_file + ", no words read");
        }
    }
    if (target != "")
    {
        spdlog::debug("Adding target [{}] to list.", target);
        target_list.push_back(target);
    }
    if (target_list_file != "")
    {
        target_list = read_file(target_list_file);
        if (target_list.size() == 0)
        {
            error_fatal("Please check " + target_list_file + ", no words read");
        }
    }

    if (exclude_list_file != "")
    {
        exclude_list = read_file(exclude_list_file);
    }
}

vector<string> read_file(string filename)
{
    try
    {
        spdlog::debug("Reading words from {} ", filename);

        vector<string> list;

        ifstream file;
        file.open(filename);
        if (file.fail())
        {
            file.close();
            error_fatal("Can not read " + filename);
        }
        string line;
        while (getline(file, line))
        {
            list.push_back(line);
        }
        file.close();

        spdlog::debug("Sucessfuly read {} words from {}", list.size(), filename);

        return list;
    }
    catch (const std::exception &e)
    {
        error_fatal("Error while reading " + filename);
    }
}
void error_fatal(string message)
{
    spdlog::error("ERROR: {}. Exiting", message);
    exit(1);
}