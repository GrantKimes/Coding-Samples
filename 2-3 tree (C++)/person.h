#ifndef PERSON_H
#define PERSON_H

#include <iostream>
#include <string>
#include <set>
#include <tuple>

using namespace std;

class person {//: less<person>{
  protected:
	int ssNum;
	struct date {
		int year, month, day;
		date (int d) {
			year = d % 10000;
			month = d / 10000 % 100;
			day = d / 1000000;
		}
	};
	date dob;
	string fName, lName, state;
	double bankBal;

  public:
	person(int ss, int d, string f, string l, string st, double b);

	void print();
	string returnPrint();
	string returnShortPrint() const;
	int compare(person * p);
	string getName();
	
	// These are used as overloaded operators, since comparing pointers to objects cannot be directly overloaded.
	bool lessThan(const person * R);
	bool greaterThan(const person * R);
	bool lessThanOrEq(const person * R);
	bool greaterThanOrEq(const person * R);

	friend bool operator<(const person & R, const person & L);
	friend bool operator>(const person & R, const person & L);
	friend bool operator<=(const person & R, const person & L);
	friend bool operator>=(const person & R, const person & L);
};

#endif