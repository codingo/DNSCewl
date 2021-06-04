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
std::string range_string;
std::string target_list_file;
std::string exclude_list_file;
std::string domain_extension_file;
std::string set_list_file;
std::string append_list_file;
std::string prepend_list_file;
bool verbose_flag, subs_flag, include_flag, no_color, no_repeat_flag, one_side_flag;
int limit, level, range = 0;
vector<string> target_list;
vector<string> append_list;
vector<string> prepend_list;
vector<string> exclude_list;
vector<string> set_list;
vector<string> extension_list;
vector<string> results;

void initialize();
vector<string> read_file(string);
void error_fatal(string);

void start();
void process_append();
void process_prepend();
void process_set();
void process_extension();
void print_domain(string);
void process_range();
bool have_repeats(string);
bool is_number(const std::string &);
bool is_number_signed(const std::string &);
int count_char(string domain, char character);
vector<string> split(const string &, char);

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

Michael Skelton (@codingo_)
Luke Stephens (@hakluke)
Sajeeb Lohani (@sml555_)                                                        
	)sep";
	spdlog::info("{}\n", header);
}

int main(int argc, char **argv)
{
	print_header();

    CLI::App app{"When provided with a list of domain names, generate a wordlist of potential subdomains to be tested for."};
    app.add_option("-t,--target", target, "Specify a single target.");
    app.add_option("-l,--tL,--target-list", target_list_file, "Specify a list of targets.");
    app.add_option("--sL,--set-list", set_list_file, "Specify a list of targets.");
    app.add_option("-e,--eL,--exclude-list", exclude_list_file, "Specify a list of targets to exclude.");
    app.add_option("--eX,--domain-extension", domain_extension_file, "Specify a list of domain extensions to substitute with.");
    app.add_option("-a,--append-list", append_list_file, "Specify a file of words to append to a host.");
    app.add_option("-p,--prepend-list", prepend_list_file, "Specify a file of words to prepend  to a host.");
    app.add_flag("-v,--verbose_flag", verbose_flag, "If set then verbose_flag output will be displayed in the terminal.");
    app.add_flag("-i,--include-original", include_flag, "If set, original domains (from source files) are included in the output.");
    app.add_option("--range", range_string, " Set a higher range for integer permutations.");
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
    vector<string> words = split(domain, '.');
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
void process_range()
{
    spdlog::debug("Processing range.");
    for (auto target = target_list.begin(); target != target_list.end(); ++target)
    {
        vector<string> splits = split(*target, '.');
        for (auto split = splits.begin(); split != splits.end(); ++split)
        {
            if (is_number(*split))
            {
		int n;
            	try {
            		n = stoi(*split);
            	}
            	catch (...) {
            		continue;
            	}
                //int n = stoi(*split);    
                int lower = 0;
                int upper = 0;
                if (range == 0)
                {
                    lower = n - 100;
                    upper = n + 100;
                }
                else if (range > 0 && !one_side_flag)
                {
                    lower = n - range;
                    upper = n + range;
                }
                else if (range < 0 && !one_side_flag)
                {
                    lower = n + range;
                    upper = n - range;
                }
                else if (range > 0)
                {
                    lower = n;
                    upper = n + range;
                }
                else if (range < 0)
                {
                    lower = n + range;
                    upper = n;
                }

                
                for (int i = lower; i < upper; i++)
                {
                    string s = to_string(i);
                    string domain = *target;
                    string temp = *split;
                    domain.replace(domain.find(temp), temp.length(), s);
                    print_domain(domain);
                }
            }
        }
    }
    cout << "Range string: " << range_string << endl;
    cout << "Range : " << range << endl;
    cout << "Both side flag: " << one_side_flag << endl;
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

                // First level
                char a[1024] = {};
                strncpy(a, targ.c_str(), location);
                strcat(a, "-");
                char temp[1024] = {};
                strncpy(temp, appnd.c_str(), appnd.length());
                strcat(a, temp);
                strcat(a, targ.substr(location, targ.length() - location).c_str());
                print_domain(string(a));

                // Second level
                if (level != 1)
                {
                    char b[1024] = {};
                    strncpy(b, targ.c_str(), location);
                    strcat(b, ".");
                    char temp1[1024] = {};
                    strncpy(temp1, appnd.c_str(), appnd.length());
                    strcat(b, temp1);
                    strcat(b, targ.substr(location, targ.length() - location).c_str());
                    print_domain(string(b));
                }
                // Third level
                char c[1024] = {};
                strncpy(c, targ.c_str(), location);
                char temp2[1024] = {};
                strncpy(temp2, appnd.c_str(), appnd.length());
                strcat(c, temp2);
                strcat(c, targ.substr(location, targ.length() - location).c_str());
                print_domain(string(c));
            }
        }
    }
}
bool is_number(const std::string &word)
{
    return !word.empty() && std::find_if(word.begin(),
                                         word.end(), [](char c) { return !std::isdigit(c); }) == word.end();
}

bool is_number_signed(const std::string &word)
{
    if (!word.empty() && (word[0] == '+' || word[0] == '-'))
    {
        for (int i = 1; i < word.length(); i++)
        {
            if (!std::isdigit(word[i]))
            {
                return false;
            }
        }
        one_side_flag = true;
    }
    else
    {
        return is_number(word);
    }
    return true;
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

void process_extension()
{
    spdlog::debug("Processing extension list.");

    for (auto target = target_list.begin(); target != target_list.end(); ++target)
    {
        for (auto extension = extension_list.begin(); extension != extension_list.end(); ++extension)
        {
            string domain = *target;
            domain.replace(domain.find_last_of("."), domain.length(), *extension);
            print_domain(domain);
        }
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
                strncpy(a, prep.c_str(), prep.length());
                strcat(a, targ.c_str());

                print_domain(string(a));

                // Second one
                if (level != 1)
                {
                    char b[1024] = {};
                    strncpy(b, prep.c_str(), prep.length());
                    strcat(b, "-");
                    strcat(b, targ.c_str());

                    print_domain(string(b));
                }
                // Third one
                char c[1024] = {};
                strncpy(c, prep.c_str(), prep.length());
                strcat(c, ".");
                strcat(c, targ.c_str());

                print_domain(string(c));
            }
        }
    }
}
void process_set()
{
    spdlog::debug("Processing set list.");
    for (int i = 0; i < target_list.size(); i++)
    {
        vector<string> splits = split(target_list[i], '.');
        for (int j = 0; j < splits.size(); j++)
        {
            if (std::find(set_list.begin(), set_list.end(), splits[j]) != set_list.end())
            {
                for (auto set = set_list.begin(); set != set_list.end(); ++set)
                {
                    if (*set != splits[j] || include_flag)
                    {
                        string subset = target_list[i];
                        subset.replace(subset.find(splits[j]), splits[j].length(), *set);
                        print_domain(subset);
                    }
                }
            }
        }
    }
}

vector<string> split(const string &domain, char delimeter)
{
    vector<string> result;
    stringstream ss(domain);
    string word;

    while (getline(ss, word, delimeter))
    {
        result.push_back(word);
    }

    return result;
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
    if (set_list.size() > 0)
    {
        process_set();
    }
    if (extension_list.size() > 0)
    {
        process_extension();
    }
    spdlog::debug(level);
    if (level == 2 || range_string != "")
    {
        process_range();
    }
}

void initialize()
{
    one_side_flag = false;
    if (verbose_flag)
    {
        spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    }
    if (range_string != "")
    {
        if (is_number_signed(range_string))
        {
            range = stoi(range_string);
        }
        else
        {
            error_fatal(range_string + " is not a valid range.");
        }
    }
    if (target == "" && target_list_file == "")
    {
        error_fatal("At least single target is needed to process");
    }
    if (append_list_file == "" && prepend_list_file == "" && set_list_file == "")
    {
        error_fatal("File name of words to process a set list, append or prepend to targets is needed");
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
            error_fatal("Please check " + target_list_file + ", no targets read");
        }
    }

    if (exclude_list_file != "")
    {
        exclude_list = read_file(exclude_list_file);
    }

    if (set_list_file != "")
    {
        set_list = read_file(set_list_file);
    }
    if (domain_extension_file != "")
    {
        extension_list = read_file(domain_extension_file);
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
