/* ECE 511 HW5: Square Fractal
 * Grant Kimes
 * 3/16/16
 */

#include "library.h"
#include <vector>

struct prevIterationsVector 
{
	vector<string> iterVec;			// Vector of strings representing directions for drawing that corresponding iteration. 
	vector<string> reverseIterVec; 
	int N;							// Final iteration number to draw.

	prevIterationsVector(int iterNumToCalculate) {
		N = iterNumToCalculate;
		if (N <= 1) return;			// First iteration is blank. 

		iterVec = vector<string>(N+1);
		reverseIterVec = vector<string>(N+1);
		iterVec[2] = "dRdRd";	// Base case for iteration 2.

		for (int ithIter = 3; ithIter <= N; ithIter++) {		// Bottom up approach to calculate iterations up to requested iteration. 
			setDirections(ithIter);
		}
	}

	void setDirections(int iterNum) {	// Algorithm. 
		string curr = "";
		curr += "R";
		curr += getPrevIterInReverse(iterNum);
		curr += "Rd";
		curr += getPrevIter(iterNum);
		curr += "LdL";
		curr += getPrevIter(iterNum);
		curr += "dR";
		curr += getPrevIterInReverse(iterNum);
		curr += "R";
		
		iterVec[iterNum] = curr;
	}

	string getPrevIter(int iterNum) {
		return iterVec[iterNum-1];
	}

	string getPrevIterInReverse(int iterNum) {
		if (reverseIterVec[iterNum-1] == "") {		// If has not yet been calculated, calculate it then save it. 
			string prevIter = getPrevIter(iterNum);
			string result = "";
			for (int i = 0; i < prevIter.size(); i++) {	// Swap all L and R directions from previous iteration.
				char c = prevIter[i];
				if (c == 'd') result += 'd';
				if (c == 'R') result += 'L';
				if (c == 'L') result += 'R';
			}
			reverseIterVec[iterNum-1] = result; 
		}

		return reverseIterVec[iterNum-1];
	}

	void draw() {
		int side = 800;
		make_window(side + 100, side + 100);
		move_to(50, side+50);
		double sideLength = side / (pow(2, N-1.0) - 1);
		if (N <= 1) return;

		string curr = iterVec[N];
		for (int i = 0; i < curr.size(); i++) {
			char c = curr[i];
			if (c == 'd') draw_distance(sideLength);
			if (c == 'R') turn_right_by_degrees(90);
			if (c == 'L') turn_left_by_degrees(90);
		}
	}

};

void main()
{
	cout << "Input an iteration number of the fractal to draw: ";
	int iterNum;
	cin >> iterNum;

	prevIterationsVector fractal(iterNum); // Bottom up calculation to find inputted iteration. 
	fractal.draw();
	
}
