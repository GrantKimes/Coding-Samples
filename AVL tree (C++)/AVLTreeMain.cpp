/* AVL Tree Measurements
 * Grant Kimes
 * 3/19/16
 */

#include <iostream>
#include <fstream>
#include <string> 
#include "person.h"

using namespace std;
#define MAX(a, b) ((a > b) ? a : b)

  template <typename T>
class Node {
  protected:
	T * data;
	Node * L, * R;
	int height;

	void rotateRight() {
		T * temp = data;
		data = L->data;
		L->data = temp;
		Node * rightMost = R;
		R = L;
		L =  R->L;
		R->L = R->R;
		R->R = rightMost;

		R->updateHeight();
		updateHeight();
	}

	void rotateLeft() {
		T * temp = data;
		data = R->data;
		R->data = temp;
		Node * leftMost = L;
		L = R;
		R =  L->R;
		L->R = L->L;
		L->L = leftMost;

		L->updateHeight();
		updateHeight();
	}

  public:
	Node(T * d, Node * l = NULL, Node * r = NULL) {
		data = d;
		L = l;
		R = r;
		height = 1;
	}

	void setData(T * d)
	 {	data = d; }
	void setR(Node * n)
	 {	R = n; }
	void setL(Node * n)
	 {	L = n; }

	int getHeight() 
	 {	return (this == NULL) ? 0 : height; }
	int getBalance() 		// Positive result means right is higher, negative means left is higher. 
	 {	return R->getHeight() - L->getHeight(); }
	T * getData() 
	 {	return data; }
	Node * getL() 
	 {	return L; }
	Node * getR() 
	 {	return R; }
	
	void updateHeight() 
	{	height = 1 + MAX(L->getHeight(), R->getHeight()); }

	void checkBalance() {
		if (getBalance() == -2) {		// Left is higher than right.
			if (L->getBalance() == 1)
				L->rotateLeft();
			rotateRight();
		} 
		else if (getBalance() == 2) {	// Right is higher than left.
			if (R->getBalance() == -1)
				R->rotateRight();
			rotateLeft();
		}
	}
};

class avlTree {
  protected:
	typedef Node<person> node;	
	node * root;

	void insert(person * P, node * currNode) {		// Recursive insertion to tree. 
		if (root == NULL) {
			root = new node(P); 
			return;
		}
		person * PinCurrNode = currNode->getData();
		if (PinCurrNode->compare(P) == 1) {			// P is bigger or equal, insert to right.
			node * R = currNode->getR();
			if (R == NULL)
				currNode->setR(new node(P));
			else 
				insert(P, R);
		}
		else if (PinCurrNode->compare(P) == -1) {	// P is smaller, insert to left.
			node * L = currNode->getL();
			if (L == NULL)
				currNode->setL(new node(P));
			else
				insert(P, L);
		}
		currNode->updateHeight();
		currNode->checkBalance();
	}

	void printTreeRecurs(node * curr) {
		if (curr == NULL) return;
		cout << "<";
		printTreeRecurs(curr->getL());
		cout << curr->getData()->getName();
		printTreeRecurs(curr->getR());
		cout << ">";
	}

	int calcDepth(node * curr) {
		return root->getHeight() - curr->getHeight() + 1;
	}

	void calcAvgHeightRecurs(node * curr, int & sum, int & count) {
		if (curr->getL()->getHeight() > 0) {
			sum += calcDepth(curr->getL());
			count++;
			calcAvgHeightRecurs(curr->getL(), sum, count);
		}
		if (curr->getR()->getHeight() > 0) {
			sum += calcDepth(curr->getR());
			count++;
			calcAvgHeightRecurs(curr->getR(), sum, count);
		}
	}

  public:
	avlTree() {
		root = NULL;
	}

	void readFile(string filename) {
		cout << "Making AVL Tree from " << filename << ":";
		ifstream fin(filename);
		int ss, d;
		string f, l, st;
		double b;
		while (true) {
			fin >> ss >> d >> f >> l >> st >> b;
			if (fin.fail())
				break;
			person * p = new person(ss, d, f, l, st, b);
			insert(p, root);
		}
		fin.close();
		cout << " finished." << endl;
	}

	void printTree() {
		cout << "Tree: " << endl;
		printTreeRecurs(root);
		cout << endl;
	}

	int getMaxDepth() {
		return root->getHeight();
	}

	double getAvgDepth() {
		int totalSum = 0;
		int count = 0;
		calcAvgHeightRecurs(root, totalSum, count);
		return (double) totalSum / count;
	}
};


int main()
{
	avlTree tree;
	tree.readFile("data1.txt");

	cout << "Average depth of all nodes in tree: " << tree.getAvgDepth() << endl;
	cout << "Greatest depth of any node (height of root): " << tree.getMaxDepth() << endl;

	cin.get();
	return 0;
}
