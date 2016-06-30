/*		Link template.
 *		Definitions in header file because of issues of compiling templates with a separate .cpp file.
 */

#ifndef __LINK_H__
#define __LINK_H__

#include <string.h>
#include <string>
#include <iostream>
using namespace std;

template <typename T> 
class Link
{
  protected:
	T * data;
	Link * next;

  public:

	Link(T * d, Link * ne = NULL)
	{
		data = d;
		next = ne;
	}

	~Link()
	{
		delete data;
		delete next;
	}

	Link * get_next()
	{	return next; }

	T * get_data()
	{	return data; }

	void print_link()
	{	data -> print();  }
};

#endif