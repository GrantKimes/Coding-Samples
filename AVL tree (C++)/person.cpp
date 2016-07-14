#include "person.h"

person::person(int ss, int d, string f, string l, string st, double b)
	  : ssNum(ss), dob(d), fName(f), lName(l), state(st), bankBal(b) {}

void person::print() {
	cout << fName << " " << lName << endl;
	//cout << ", " << ssNum << " (" << dob.day << " " << dob.month << " ";
	//cout << dob.year << ") " << state << " " << bankBal << endl;
}

string person::returnPrint() {
	return fName + " " + lName;
}

string person::returnShortPrint() const {
	return lName.substr(0, 3);
}

string person::getName() {
	return lName.substr(0, 4);
}

int person::compare(person * p) {	// Return -1 if p is smaller, 1 if p is bigger or equal. 
	if (lName < p->lName)
		return 1;
	else if (lName > p->lName) 
		return -1;
	else if (lName == p->lName) {
		if (fName < p->fName)
			return 1;
		else if (fName < p->fName)
			return -1;
		else
			return 1;
	}
}

bool person::lessThan(const person * R) {
	return (*this < *R);
}
bool person::greaterThan(const person * R) {
	return (*this > *R);
}
bool person::lessThanOrEq(const person * R) {
	return (*this <= *R);
}
bool person::greaterThanOrEq(const person * R) {
	return (*this >= *R);
}

bool operator<(const person & L, const person & R) {
	return tie(L.lName, L.fName) < tie(R.lName, R.fName);
	if (L.lName < R.lName) 
		return true;
	if (L.lName > R.lName) 
		return false;
	if (L.fName < R.fName)
		return true;
	if (L.fName > R.fName)
		return false;
	return false;
}

bool operator>(const person & L, const person & R) {
	return (R <= L);
}
bool operator<=(const person & L, const person & R) {
	return !(R > L);
}
bool operator>=(const person & L, const person & R) {
	return !(L < R);
}