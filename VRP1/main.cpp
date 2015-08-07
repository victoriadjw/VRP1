#include "VehicleRouting.h"
#include"Input.h"
#include"Output.h"
#include"Solver.h"
using namespace std;

const std::string ARGV_HELP;
void help()
{
	cout <<
		"command line arguments ([XXX] means XXX is optional) :\n"
		"  [id]      - identifier of the run which will be recorded in log file.\n"
		"  sce       - scenario file path.\n"
		"  his       - history file path.\n"
		"  week      - weekdata file path.\n"
		"  sol       - solution file path.\n"
		"  [cusIn]   - custom input file path.\n"
		"  [cusOut]  - custom output file path.\n"
		"  [rand]    - rand seed for the solver.\n"
		"  [timeout] - max running time of the solver.\n"
		"  [iter]    - max iteration count of the solver.\n"
		"              reaching either timeout or iter will end the program.\n"
		"  [config]  - specifies algorithm select and argument settings.\n"
		"              format: cci;d;d,d,d,d;d,d,d,d\n"
		"                  c for char, d for real number,\n"
		"                  comma is used to separate numbers.\n"
		"                  the first char can be:\n"
		"                      'g'(greedy init) or 'e'(exact init).\n"
		"                  the second char can be:\n"
		"                      'w'(Random Walk), 'i'(Iterative Local Seedgeh),\n"
		"                      'p'(Tabu Seedgeh Possibility), 'l'(TS Loop),\n"
		"                      'r'(TS Rand), 's'(Swap Chain) or 'b'(Bias TS).\n"
		"                  i is a non-negative integer in enum Solution::ModeSeq.\n"
		"                  next real number is the coefficient of no improve count.\n"
		"                  following 4 real numbers are coefficients for TableSize,\n"
		"                  NurseNum, WeekdayNum and ShiftNum used in day tabu\n"
		"                  tenure setting, non-positive number means there is \n"
		"                  no relation with that feature. while next 4 numbers are\n"
		"                  used in shift tabu tenure setting with same meaning.\n"
		"              example: gt2;1.5;0,0.5,0,0;0,0.8,0,0\n"
		"                       gt3;0.8;0.1,0,0,0;0.1,0,0,0\n"
		<< endl;
}

int run(int argc, char *argv[])
{
	char *rgv[] = { "",//0
		"-p", "13",//1,2
		"-ws", "0",//3,4
		"-xa", "500",//5,6
		"-xb", "300",//7,8
		"-xc", "100",//9,10
		"-alpha", "700",//11,12
		"-beta", "250",//13,14
		"-qa", "1000",//15,16
		"-qb", "5000",//17,18
		"-help"//, ""
	};
	argv = rgv;
	argc = sizeof(rgv) / sizeof(rgv[0]);
	map<string, string> argvMap;
	int j;
	for (int i = 1; i < argc; i = j)
	{
		if (argv[i][0] == '-')
		{
			argvMap[argv[i] + 1];
		}
		for (j = i + 1; (j < argc) && (argv[j][0] != '-'); j++)
		{
			argvMap[argv[i] + 1] += argv[j];
		}
	}
	for (std::map<string, string>::iterator iter = argvMap.begin(); iter != argvMap.end(); iter++)
	{
		cout << iter->first << " -> " << iter->second << endl;
	}
	//print help
	if (argvMap.find(ARGV_HELP) != argvMap.end())
	{
		help();
	}
	return 0;

}
#if 1
int main(int argc, char *argv[])
{
	//return run(argc, argv);
	cout << "vehicle routing problem." << endl;
	//int seed = time(NULL);
	int seed = 1438913349;
	srand(seed);
	//run(argc, argv);
	//int myints[] = { 10, 20, 30, 40, 50, 60, 70 };
	std::vector<int> myints;
	for (int i = 0; i < 7; i++)
		myints.push_back(i + 1);
	std::vector<int> myvector(7);

	std::copy(myints.begin(), myints.end(), myvector.begin());

	Timer t(0, 31, 22);
	std::chrono::seconds difft = t.restTime();
	cout << difft.count() << endl;
	int temp_i = 10;
	const int &rin = temp_i;
	cout << rin << endl;
	temp_i -= 1;
	cout << rin << endl;
	const int &rin1 = 40;
	const int &rin2 = temp_i * 2;
	cout << rin1 << " " << rin2 << endl;
	VehicleRouting vr;
	Input in("vrptwcdc//case1-A-project.vrp", "results//log.txt", vr);
	Output op("results//log.txt", vr);
	op.ofs << seed << endl;
	vr.modifyOrder();
	vr.setMandOptionOrder();
	Solver solver(vr);
	solver.initSolution(1);
	system("pause"); 
}
#endif