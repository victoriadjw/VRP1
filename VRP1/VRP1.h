/**



*/

#ifndef VRP1_H
#define VRP1_H

#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<sstream>
#include<cstdlib>
#include<ctime>

using namespace std;

namespace VRP1
{
	const int MAX_BUF_SIZE = 1000;	// max size for char array buf
	extern const string LOG_FILE_NAME;	// name of log file

	void help();
	int run(int argc, char *argv[]);

	bool readScenario(const string scenarioFileName);
}
#endif