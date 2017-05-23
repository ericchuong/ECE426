#include <iostream>
#include <vector>
using namespace std;

double FCFS(int n, int a[]);
double SSTF(int n, int a[]);
int* sort(int n, int a[]);
double SCAN(int n, int a[], int current, int direction);
double variance(int n, int a[], double average);

void main()
{
	int a[] = { 63, 33, 72, 47, 8, 99, 74, 52, 75 };
	int n = (sizeof a / sizeof a[0]);
	double aFCFS, aSSTF, aSCAN;

	cout << "a[] = { 63, 33, 72, 47, 8, 99, 74, 52, 75 }\n";
	aFCFS = FCFS(n, a);
	cout << "Average move for FCFS is " << aFCFS << "\n";
	cout << "Variance for FCFS is " << variance(n, a, aFCFS) << "\n\n";

	aSSTF = SSTF(n, a);
	cout << "Average move for SSTF is " << aSSTF << "\n";
	cout << "Variance for SSTF is " << variance(n, a, aSSTF) << "\n\n";

	aSCAN = SCAN(n, a, 63, 1);
	cout << "Average move for decreasing SCAN is " << aSCAN << "\n";
	aSCAN = SCAN(n, a, 63, 0);
	cout << "Average move for increasing SCAN is " << aSCAN << "\n";
	cout << "Variance for SCAN is " << variance(n, a, aSCAN) << "\n\n";
	

	cout << "\n\nb[] = random 100 integers";
	int b[100];
	int bsize = (sizeof b / sizeof b[0]);
	double bFCFS, bSSTF, bSCAN;

	for (int i = 0; i < bsize; i++)
	{
		b[i] = rand() % 100;
	}
	cout << "\n";
	bFCFS = FCFS(n, b);
	cout << "Average move for FCFS is " << bFCFS << "\n";
	cout << "Variance for FCFS is " << variance(bsize, b, bFCFS) << "\n\n";

	bSSTF = SSTF(bsize, b);
	cout << "Average move for SSTF is " << bSSTF << "\n";
	cout << "Variance for SSTF is " << variance(bsize, b, bSSTF) << "\n\n";

	bSCAN = SCAN(bsize, b, 41, 1);
	cout << "Average move for decreasing SCAN is " << bSCAN << "\n";
	bSCAN = SCAN(bsize, b, 41, 0);
	cout << "Average move for increasing SCAN is " << bSCAN << "\n";
	cout << "Variance for SCAN is " << variance(bsize, b, bSCAN) << "\n\n";


	cout << "\n\nc[] = random 1000 integers";
	int c[1000];
	int csize = (sizeof c / sizeof c[0]);
	double cFCFS, cSSTF, cSCAN;

	for (int i = 0; i < csize; i++)
	{
		c[i] = rand() % 100;
	}
	cout << "\n";
	cFCFS = FCFS(csize, c);
	cout << "Average move for FCFS is " << cFCFS << "\n";
	cout << "Variance for FCFS is " << variance(csize, c, cFCFS) << "\n\n";

	cSSTF = SSTF(csize, c);
	cout << "Average move for SSTF is " << cSSTF << "\n";
	cout << "Variance for SSTF is " << variance(csize, c, cSSTF) << "\n\n";

	cSCAN = SCAN(csize, c, 41, 1);
	cout << "Average move for decreasing SCAN is " << cSCAN << "\n";
	cSCAN = SCAN(csize, c, 41, 0);
	cout << "Average move for increasing SCAN is " << cSCAN << "\n";
	cout << "Variance for SCAN is " << variance(csize, c, cSCAN) << "\n\n";

	system("pause");
}

double FCFS(int n, int a[])
{
	// Purpose: to implement First-Come-First-Serve disk scheduling
	// Input:   n = size of a[]
	//			a[] = { 63, 33, 72, 47, 8, 99, 74, 52, 75 };
	// Output:  average move

	double sum, averageMove;
	sum = 0;

	for (int i = 1; i < n; i++)
	{
		sum = sum + abs((a[i] - a[i - 1]));
	}
	averageMove = sum / (n - 1);
	return averageMove;
}

double SSTF(int n, int a[])
{
	// Purpose: to implement First-Come-First-Serve disk scheduling
	// Input:   n = size of a[]
	//			a[] = { 63, 33, 72, 47, 8, 99, 74, 52, 75 };
	// Output:  average move

	double sum, averageMove;
	sum = 0;
	int j;
	int track = a[0];
	vector <int> v;
	sort(n, a);

	for (int i = 0; i < n; i++)
	{	
		v.push_back(a[i]);
	}
	for (j = 0; j < n; j++)
	{
		if (a[j] == track)
			break;
	}
	for (int k = 0; k < (n - 1); k++)
	{
		if (j + 1 == v.size())
			goto here;
		// move right
		if ((j == 0) || (abs(v[j] - v[j - 1])) >= (abs(v[j + 1] - v[j])))
		{
			sum = sum + (abs(v[j + 1] - v[j]));
			track = v[j + 1];
			v.erase(v.begin() + j);
		}
		//move left
		else if ((abs(v[j] - v[j - 1])) < (abs(v[j + 1] - v[j])))
		{
			here:;
			sum = sum + (abs(v[j] - v[j - 1]));
			track = v[j - 1];
			v.erase(v.begin() + j);
			j = j - 1;
		}
	}
	averageMove = sum / (n - 1);
	return averageMove;
}

int* sort(int n, int a[])
{
	// Purpose: to sort the array to increasing order
	// Input:   n = size of a[]
	//			a[] = { 63, 33, 72, 47, 8, 99, 74, 52, 75 };
	// Output:  a[] = { 8, 33, 47, 52, 63, 72, 74, 75, 99 };

	vector <int> v;
	vector <int> temp;
	int counter = 0;
	int j, jHold;

	for (int i = 0; i < n; i++)
	{
		temp.push_back(a[i]);
	}
	for (int i = 0; i < n; i++)
	{
		int smallestNum = 100;
		for (j = 0; j < temp.size(); j++)
		{
			if (temp[j] < smallestNum)
			{
				smallestNum = temp[j];
				jHold = j;
			}
		}
		v.push_back(smallestNum);
		temp.erase(temp.begin() + jHold);
	}
	for (int i = 0; i < n; i++)
	{
		a[i] = v[i];
	}
	return a;
}

double SCAN(int n, int a[], int current, int direction)
{
	// Purpose: to implement SCAN disk scheduling
	// Input:   n = size of a[]
	//			a[] = { 63, 33, 72, 47, 8, 99, 74, 52, 75 };
	//			current = current head track number (63)
	//			direction = 1, decreasing; 0, increasing
	// Output:  average move

	double averageMove, sum;
	int i, j;
	sum = current - a[0];
	sort(n, a);

	for (i = 0; i < n; i++)
	{
		if (a[i] == current)
			break;
	}
	if (direction == 1)
	{
		for (j = i; j > 0; j--)
		{
			sum = sum + a[j] - a[j - 1];
			current = a[j - 1];
		}
		for (j = i; j < (n - 1); j++)
		{
			sum = sum + a[j + 1] - a[j];
			current = a[j + 1];
		}
	}
	else
	{ 
		for (j = i; j < (n - 1); j++)
		{
			sum = sum + a[j + 1] - a[j];
			current = a[j + 1];
		}
		for (j = i; j > 0; j--)
		{
			sum = sum + a[j] - a[j - 1];
			current = a[j - 1];
		}
	}
	averageMove = sum / (n - 1);
	return averageMove;
}

double variance(int n, int a[], double average)
{
	// Purpose: finds the variance
	// Input:   n = size of a[]
	//			a[] = { 63, 33, 72, 47, 8, 99, 74, 52, 75 };
	//			average = averageMove of a disk scheduling algorithm
	// Output:  variance
	double var = 0;
	double temp = 0;

	for (int i = 1; i < n; i++)
	{
		var = var + ((abs(a[i] - a[i - 1]) - average) * (abs(a[i] - a[i - 1]) - average));
	}
	var = var / (n-1);
	return var;
}
