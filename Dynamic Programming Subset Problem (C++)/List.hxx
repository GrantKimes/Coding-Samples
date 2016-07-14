/*		List template.
 *		Definitions in header file because of issues of compiling templates with a separate .cpp file.
 */

#ifndef __LIST_H__
#define __LIST_H__

#include <algorithm>
#include "Link.hxx"

template <typename T>
class List
{
  protected:
	Link<T> * first;
	int length;

  public:
	List();
	~List();
	int get_length();
	Link<T> * get_first();
	bool is_empty();
	void insert(T * d);
	bool print_list(string s);
	bool in_list(string s);
	List<T> * return_name_matches(string name);
};

template <typename T>
List<T>::List()
{	
	first = NULL;
	length = 0;	
}

template <typename T>
List<T>::~List()
{
	Link<T> * prev = first;
	first = first -> get_next();
	while (first != NULL)
	{
		delete prev;
		prev = first;
		first = first -> get_next();
	}
	delete first;
}

template <typename T>
int List<T>::get_length()
{	return length;	}

template <typename T>
Link<T> * List<T>::get_first()
{	return first;	}

template <typename T>
bool List<T>::is_empty()
{	if (first == NULL) return true;
	else return false;	}

template <typename T>
void List<T>::insert(T * d)
{
	if (first == NULL)
		first = new Link<T>(d);
	else
		first = new Link<T>(d, first);
	length++;
}

template <typename T>
bool List<T>::print_list(string s)
{
	Link<T> * ptr = first;
	bool printed = false;
	transform(s.begin(), s.end(), s.begin(), ::tolower);

	while (ptr != NULL)
	{
		string curr_name = ptr->get_data()->get_name();
		transform(curr_name.begin(), curr_name.end(), curr_name.begin(), ::tolower);

		if (s == curr_name)
		{
			ptr -> print_link();
			printed = true;
		}
		ptr = ptr -> get_next();
	}
	return printed;
}

template <typename T>
bool List<T>::in_list(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	Link<T> * ptr = first;
	bool found = false;
	while (ptr != NULL)
	{
		string curr_name = ptr->get_data()->get_name();
		transform(curr_name.begin(), curr_name.end(), curr_name.begin(), ::tolower);
		if (s == curr_name)
			found = true;
		ptr = ptr -> get_next();
	}
	return found;
}

template <typename T>
List<T> * List<T>::return_name_matches(string name)
{
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	List<T> * new_list = new List<T>();
	Link<place> * ptr = first;
	while(ptr != NULL)
	{
		string curr_name = ptr->get_data()->get_name();
		transform(curr_name.begin(), curr_name.end(), curr_name.begin(), ::tolower);

		if (name == curr_name)
			new_list -> insert(ptr->get_data());
		ptr = ptr -> get_next();
	}
	return new_list;
}




#endif



/*
template <typename T>
class placeList
{};

template <>
class placeList<place> : public List<place>		// PLACE SPECIALIZATION CLASS
{
  protected:

	Link<place> * first;
	int length;

  public:

	//placeList();
	int get_length();
	Link<place> * get_first();

	void insert(place * d);
	bool print_list(string s);
	bool in_list(string s);

	//List<T> * return_city_matches(string name);

	place * place_in_state(string state)
		{
	transform(state.begin(), state.end(), state.begin(), ::tolower);
	Link<place> * ptr = first;
	while (ptr != NULL)
	{
		string curr_state = ptr->get_data()->get_state();
		transform(curr_state.begin(), curr_state.end(), curr_state.begin(), ::tolower);
		if (state == curr_state)
			return ptr -> get_data();
		ptr = ptr -> get_next();
	}
	return NULL;
};

	//template<> class List<place>;

};*/

/*
template <>
place * List<place>::place_in_state(string state)
{
	transform(state.begin(), state.end(), state.begin(), ::tolower);
	Link<place> * ptr = first;
	while (ptr != NULL)
	{
		string curr_state = ptr->get_data()->get_state();
		transform(curr_state.begin(), curr_state.end(), curr_state.begin(), ::tolower);
		if (state == curr_state)
			return ptr -> get_data();
		ptr = ptr -> get_next();
	}
	return NULL;
}
*/



/*

template<>
List<place>::List<place>()
{	testlist = 99;	}
*/