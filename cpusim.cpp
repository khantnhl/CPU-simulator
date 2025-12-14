#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include<fstream>
#include <sstream>
using namespace std;


int main(int argc, char* argv[])
{


	// instruction memo
	char instMem[4096] = {0};


	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		// cout<<"error opening file\n";
		return 0; 
	}

	string line; 
	int i = 0;
	while (infile >> line && i < 4096) {
			
			stringstream line2(line);
			unsigned int x; 
			line2 >> hex >> x;
			instMem[i] = static_cast<char>(x); // be careful about hex
			i++;

		}

	int maxPC= i/4; // max number of instructions
	// cout<< "maxPC: " << maxPC << endl;



	CPU myCPU(instMem);  // call the approriate constructor here to initialize the processor...  
	

	int cycle = 0;
	bool debug = false;

	while (true) // main loop. Each iteration is equal to one clock cycle.  
	{
		
		//fetch
		uint32_t instruction = myCPU.fetch();

		if(instruction == 0){
			break;
		}
		
		// decode
		myCPU.decode(instruction);

		// execute
		myCPU.execute();
        
		// Mem
		myCPU.mem();

		// Write Back
        myCPU.wb();

		// Debugging purposes
		if(debug){
			myCPU.print_debug_state(instruction, cycle);
		}

		myCPU.updatePC(); // Update PC


		cycle++;

		if (myCPU.readPC() > maxPC * 4)
			break;
	}

	int a0 = myCPU.getA0();
	int a1 = myCPU.getA1();  
	
	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	cout << "(" << a0 << "," << a1 << ")" << endl;
	
	return 0;

}