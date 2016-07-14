/*	Subset Problem
 *	Grant Kimes
 *	ECE511 Assignemt 3
 *	2/7/16
 *	Given an array of numbers and a goal number, find a subset that adds to that number. 
 */

#include <iostream>
#include <vector>
#include <string>
#include <time.h>

using namespace std;

class subsetFinder
{
  protected:
	int goalNum;							// Number to sum to with a subset. 
	vector<int> inputs;						// Vector of inputted values to find a subset of.
	int numInputs;							// Number of inputted values.
	vector< vector<bool>> savedResults;		// Saved previous results to prevent recursions that have already been done.
	vector<bool> result;					// Boolean array whose indexes indicate which inputs are used in the resulting subset vector.

	vector<bool> find_subset(int desiredSum, int currIndex);

  public:
	subsetFinder();
	subsetFinder(int g, vector<int> in);
	void find_subset();
	void print_result();
};

subsetFinder::subsetFinder()	// Generates random input vector and goal number to test.
{
	int maxGoalNum = 1000;
	int maxNumInputs = 100;
	
	srand(time(NULL));
	goalNum = rand() % maxGoalNum;
	numInputs = rand() % maxNumInputs;
	for (int i = 0; i < numInputs; i++)
		inputs.push_back(rand() % goalNum);

	savedResults.resize(numInputs);
}

subsetFinder::subsetFinder(int g, vector<int> in)	// Input desired goal number and input vector.
{
	goalNum = g;
	inputs = in;
	numInputs = inputs.size();
	savedResults.resize(numInputs);
}

void subsetFinder::find_subset()
{
	// Check if it is even possible to reach desired number before recursions happen. 
	int maxSum = 0;
	for (int i = 0; i < numInputs; i++)
		maxSum += inputs[i];
	if (maxSum < goalNum) {
		vector<bool> x;
		result = x;
		return;
	}

	result = find_subset(goalNum, 0);		// Call recursive subset function, starting with the goal number and index 0 (looking at the whole array).
}

vector<bool> subsetFinder::find_subset(int desiredSum, int currIndex)	// Recursively finds a subset solution of inputs[], from currIndex to the end of the vector, adding to desiredSum. 
{
	vector<bool> result;

	// Base case. Including current number results in successfully adding to desired number. Return boolean index array indicating currIndex is used in solution.
	if (desiredSum == 0) {
		result.resize(numInputs);
		return result;
	}
	// Not a solution. Including current number either is too large and makes desiredSum negative, or all the inputs have been checked through.	
	if (desiredSum < 0 || currIndex == numInputs) 
		return result;

	if (savedResults[currIndex].size() > 0)		// If a subset at this index has been calculated already, use that saved result array.
		return savedResults[currIndex];

	vector<bool> isUsed = find_subset(desiredSum - inputs[currIndex], currIndex + 1);	// Check if there is a subset using current number and the rest of the numbers.
	vector<bool> notUsed = find_subset(desiredSum, currIndex + 1);						// Check if there is a subset using the rest of the numbers.

	if (isUsed.size() == 0 && notUsed.size() == 0) {	// No solution was found whether this number is used or not used. Return nothing.
		savedResults[currIndex] = result;	
		return result;
	}

	if (isUsed.size() > 0 )	{	 // There was a solution found that includes the current number. Add current index to current solution. 
		isUsed[currIndex] = 1;
		savedResults[currIndex] = isUsed;
		return isUsed;
	}
	else {		// There was a solution found that does not include the current number. Return that solution.
		savedResults[currIndex] = notUsed;
		return notUsed;
	}
}

void subsetFinder::print_result()
{
	cout << "Subset of { ";
	for (int i = 0; i < numInputs; i++){
		cout << inputs[i];
		if (i < numInputs-1) 
			cout << ", ";
	}
	cout << "} " << " that adds to " << goalNum << ":" << endl << endl;

	if (result.size() == 0) {
		cout << "No solution found " << endl;
		return;
	}

	cout << "Found subset that adds to " << goalNum << ": { ";
	for (int i = 0; i < numInputs; i++) {
		if (result[i] == 1) 		// If this value is set in result vector, that corresponding index in the input vector is used in the solution.
			cout << inputs[i] << ", ";
	}
	cout << "} " << endl;
}

void main()
{
	//int arr[] = {2, 4, 6, 4, 4,4,4,4};
	//int sum = 9;
	//vector<int> inputs (arr, arr + sizeof(arr) / sizeof(int));	// Initialize input vector. Inputted as array because of easier bracket syntax.
	//subsetFinder SF(sum, inputs);

	subsetFinder SF;	// Generates random test case. Constraints are defined in constructor function.

	SF.find_subset();
	SF.print_result();

	string x;
	cin >> x;	// So the console window doesn't disappear after running.
}