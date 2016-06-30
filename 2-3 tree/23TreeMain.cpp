/* 2-3 Tree Measurements
 * Grant Kimes
 * 4/10/16
 */

#include <iostream>
#include <fstream>
#include <string> 
#include "person.h"

using namespace std;
#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)

  template <typename T>
class twoThreeNode {
  protected:
	T * dataL, * dataR;
	twoThreeNode * Lchild, * Mchild, * Rchild;
	int height;

  public:
	friend class twoThreeTree;
	twoThreeNode(T * data, twoThreeNode * L = NULL, twoThreeNode * M = NULL) {
		dataL = data;
		dataR = NULL;
		Lchild = L;
		Mchild = M;
		Rchild = NULL;
		height = 1 + MAX(L->getHeight(), M->getHeight());
	}

	~twoThreeNode() {
		delete L;
		delete M;
		delete R;
	}

	T * getDataL() {
		return dataL;	}
	T * getDataR() {
		return dataR;	}
	int getHeight() {
		return (this == NULL) ? 0 : height;	}
	bool isTwoNode() {
		return dataR == NULL && dataL != NULL;	}
	bool isThreeNode() {
		return !isTwoNode();	}	
	bool isLeaf() {
		return dataL == NULL;	}

	string returnPrint() {
		if (isTwoNode()) 
			return "Two node: " + dataL->returnShortPrint();
		else
			return "Three node: " + dataL->returnShortPrint() + ", " + dataR->returnShortPrint();
	}

	void absorbIntoTwoNode(twoThreeNode * & leftFragment, T * & dataFromBelow, twoThreeNode * & rightFragment) {
		if (dataFromBelow->lessThan(dataL)) {	// Coming up from left insertion.
			dataR = dataL;
			dataL = dataFromBelow;
			Rchild = Mchild;
			Mchild = rightFragment;
			Lchild = leftFragment;
		}
		else if (dataFromBelow->greaterThanOrEq(dataL)) {	// Coming up from right insertion.
			dataR = dataFromBelow;
			Mchild = leftFragment;
			Rchild = rightFragment;
		}
		// Indicate that data was successfully absorbed, no more absorbing necessary in later recursion unwinding.
		dataFromBelow = NULL;	
		leftFragment = NULL;
		rightFragment = NULL;
	}

	void absorbIntoThreeNode(twoThreeNode * & leftFragment, T * & dataFromBelow, twoThreeNode * & rightFragment) {
		T *d1, *d2, *d3;	// Data 1, 2, 3.
		twoThreeNode *n1, *n2, *n3, *n4;	// Node 1, 2, 3, 4.

		if (dataFromBelow->lessThan(dataL)) {			// Coming up from left insertion.
			d1 = dataFromBelow;
			d2 = dataL;
			d3 = dataR;
			n1 = leftFragment;
			n2 = rightFragment;
			n3 = Mchild;
			n4 = Rchild;
		}
		else if (dataFromBelow->greaterThanOrEq(dataL) && dataFromBelow->lessThan(dataR)) {	// Coming up from middle insertion.
			d1 = dataL;
			d2 = dataFromBelow;
			d3 = dataR;
			n1 = Lchild;
			n2 = leftFragment;
			n3 = rightFragment;
			n4 = Rchild;
		}
		else if (dataFromBelow->greaterThanOrEq(dataR)) {		// Coming up from right insertion.
			d1 = dataL;
			d2 = dataR;
			d3 = dataFromBelow;
			n1 = Lchild;
			n2 = Mchild;
			n3 = leftFragment;
			n4 = rightFragment;
		}

		// Create a new two node, this is one of the fragments.
		twoThreeNode * newRight = new twoThreeNode(d3, n3, n4);
		rightFragment = newRight;

		// Reassign this's values (currently a 3 node) to become a two node. 
		dataL = d1;
		dataR = NULL;
		Lchild = n1;
		Mchild = n2;
		Rchild = NULL;

		leftFragment = this;
		dataFromBelow = d2;	// Data gets propogated up to next node for insertion.
	}
};


class twoThreeTree {
  protected:
	typedef twoThreeNode<person> node;
	node * root;

  public:
	twoThreeTree() {
		root = NULL;
	}

	void insert(person * p, node * curr, node * & leftFragment, person * & dataFromBelow, node * & rightFragment) {
		if (curr == NULL) {		// Reached a leaf node, person being inserted should be sent back up to previous recursion.
			dataFromBelow = p;
			return;
		}

		if (curr->isTwoNode()) {
			// Recursive insert left in two node. 
			if (p->lessThan(curr->dataL)) 		
				insert(p, curr->Lchild, leftFragment, dataFromBelow, rightFragment);
			
			// Recursive insert right in two node.
			else if (p->greaterThanOrEq(curr->dataL)) 
				insert(p, curr->Mchild, leftFragment, dataFromBelow, rightFragment);
			
			// Recursive insert resulted in data being pushed up to curr node, insert into curr two node.
			if (dataFromBelow != NULL) 
				curr->absorbIntoTwoNode(leftFragment, dataFromBelow, rightFragment);	// dataFromBelow is inserted, and no more balancing is required.
		} 
		else if (curr->isThreeNode()) { 
			// Recursive insert left in three node.
			if (p->lessThan(curr->dataL)) 
				insert(p, curr->Lchild, leftFragment, dataFromBelow, rightFragment);
			
			// Recursive insert middle in three node.
			else if (p->greaterThanOrEq(curr->dataL) && p->lessThan(curr->dataR)) 
				insert(p, curr->Mchild, leftFragment, dataFromBelow, rightFragment);
			
			// Recursive insert right.
			else if (p->greaterThanOrEq(curr->dataR)) 
				insert(p, curr->Rchild, leftFragment, dataFromBelow, rightFragment);

			// Recursive insert resulted in data being pushed up to curr node, insert into curr three node.
			if (dataFromBelow != NULL) 
				curr->absorbIntoThreeNode(leftFragment, dataFromBelow, rightFragment);	// dataFromBelow is inserted, and another value gets propogated to next recursion level.
		}
	}

	void add(person * p) {
		if (root == NULL) {
			root = new node(p);
		} 
		else {
			person * D = NULL;
			node * N1 = NULL;
			node * N2 = NULL;
			insert(p, root, N1, D, N2);
			if (D != NULL) {
				root = new node(D, N1, N2);
			}
		}
	}

	void readFile(string filename) {
		cout << "Making 2-3 Tree from " << filename << ":";
		ifstream fin(filename);
		if (fin.fail()) {
			cout << "Couldn't open file. " << endl;
			return;
		}

		int ss, d;
		string f, l, st;
		double b;
		while (true) {
			fin >> ss >> d >> f >> l >> st >> b;
			if (fin.fail())
				break;
			person * p = new person(ss, d, f, l, st, b);
			add(p);
		}
		fin.close();
		cout << " finished." << endl;
	}

	void print() {
		cout << "PRINTING TREE: Height of root is " << getMaxDepth() << endl;
		printRecurs(root);
		cout << endl << endl;
	}

	void printRecurs(node * curr) {
		if (curr == NULL) return;
		cout << "(";
		if (curr->isThreeNode()) {
			printRecurs(curr->Lchild);
			cout << curr->dataL->returnShortPrint() << "[";
			printRecurs(curr->Mchild);
			cout << "]" << curr->dataR->returnShortPrint();
			printRecurs(curr->Rchild);
			
		}
		else if (curr->isTwoNode()) {
			printRecurs(curr->Lchild);
			cout << curr->dataL->returnShortPrint();
			printRecurs(curr->Mchild);
		}
		cout << ")";
	}

	int getMaxDepth() {
		int maxDepth = 0;
		int currDepth = 0;
		maxDepthRecurs(root, maxDepth, currDepth + 1);
		return maxDepth;
	}

	void maxDepthRecurs(node * curr, int & maxDepth, int currDepth) {
		if (curr == NULL)
			return;
		if (currDepth > maxDepth)
			maxDepth = currDepth;
		maxDepthRecurs(curr->Lchild, maxDepth, currDepth + 1);
		maxDepthRecurs(curr->Mchild, maxDepth, currDepth + 1);
		maxDepthRecurs(curr->Rchild, maxDepth, currDepth + 1);
	}

	int calcDepth(node * curr) {	// Height is saved in nodes, so depth must be calculated.
		return root->getHeight() - curr->getHeight() + 1;
	}

	void avgDepthRecurs(node * curr, double & totalDepth, double & count, int currDepth) {
		if (curr == NULL) 
			return;
		totalDepth += currDepth;
		count++;
		if (curr->isThreeNode()) {
			totalDepth += currDepth;
			count++;
		}
		avgDepthRecurs(curr->Lchild, totalDepth, count, currDepth + 1);
		avgDepthRecurs(curr->Mchild, totalDepth, count, currDepth + 1);
		avgDepthRecurs(curr->Rchild, totalDepth, count, currDepth + 1);
	}
	
	double getAvgDepth() {
		double totalDepth = 0;
		double count = 0;
		int currDepth = 0;
		avgDepthRecurs(root, totalDepth, count, currDepth + 1);
		if (count == 0) return 0; 
		return totalDepth / count;
	}
};

int main() {
	twoThreeTree tree;
	tree.readFile("data100.txt");

	cout << "Average depth of all nodes in tree: " << tree.getAvgDepth() << endl;
	cout << "Greatest depth of any node: " << tree.getMaxDepth() << endl;

	cin.get();
	return 0;
}