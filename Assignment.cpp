/*
	1. 	Implement the usual A*
		for 8-Puzzle, with the Expand function taking a “depth” parameter –
		the depth to which each expand call expands.
	2. 	Implement both the usual heuristics: tile mismatch count and manhattan distance.
	3. 	In the main function, read the depth parameter as input, get the InitialState, and call the A*
		solver twice for each heuristic: once with the input depth parameter, and then again with the
		parameter being 1 (which is the usual Expand).
		Thus the total number of times that the puzzle will be solved by A*
		is 4.
	4. 	Compare the results of all the four calls for at least 10 randomly generated initial states for
		optimality. Make sure this is done in the program itself.
	5. 	Compare the computational costs (number of nodes generated and the maximum length of the
		fringe throughout) of all the four calls for the different initial states.
*/

/*
	Algorithm 1: Modified A*
	Require: I: The Initial State
	Require: h: The Heuristic Function
	Require: depth: The BFS Depth
	1: fringe← []
	2: S ← I
	3: f ← 0
	4: while ¬Goal-Test(S) do
	5: temp←ModifiedExpand(S, depth)
	6: for each T ∈temp do
	7: UpdateFringe(fringe, T, f = h(T) + g(T))
	8: end for
	9: S ←RemoveFirst(fringe)
	10: end while
	11: return S
*/

/*
	Algorithm 2: ModifiedExpand
	Require: S: The State to be Expanded
	Require: depth: The BFS Depth
	1: temp1 ←MakeFIFOqueue(Expand(S))
	2: temp2 ← temp1
	3: d ← depth − 1
	4: while d > 0 do
		5: while ¬Empty(temp2) do
			6: C ←RemoveFirst(temp2)
			7: temp3 ←Expand(C)
			8: temp1 ←Append(temp1, temp3)
		9: end while
		10: d ← d − 1
		11: temp2 ← temp3
	12: end while
	13: return temp1
*/

/* 

		Functions used							Description

*/
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <ctime>
#include <cstdlib>


using namespace std;


/*
*	Stores the state of the puzzle at any given point. Depth represents the depth from the initial state.
*	Depth for initial state is 0.
*/
struct puzzleState{
	int positions[3][3]; 
	int depth;
	//depth from starting state
};

/*
	Used to display the puzzle State
*/
void displayState(puzzleState S){
	for(int i=0;i<3;i++){
		cout<<"\n";
		for(int j=0;j<3;j++){
			cout<<S.positions[i][j]<<"\t";
		}
	}
	cout<<"\n";
}

/*
	Helper functions to convert a 1D position to a 2D position
	eg: position 7 in the array would refer to [2][1].
	row is 8/totalColumns = 2
	col is 8%totalColumns = 1
*/

int actualRow(int num,int totalColumns){
	return num / totalColumns;
}

int actualColumn(int num, int totalColumns){
	return num % totalColumns;
}

/*
	Function used to calculate the manhattan distance heuristic for a given state of the puzzle
	Manhattan distance for one position is the sum of sideways distances to its actual position
	eg: if number 8 is at [1][0].
	Its actual position in the goal state is [2][2]	
	So manhattan distance for 8 is (2-1) + (2-0) = 3
	We find manhattan distance individually for all positions and add them up to get the heuristic for the given state.
*/

int manhattanDistance(puzzleState p){
	int distance=0;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(p.positions[i][j]!=0)
				distance += abs(i - actualRow(p.positions[i][j],3)) + abs(j - actualColumn(p.positions[i][j],3));
		}
	}
	return distance;
}
/*
	Function used to find the number of tiles that are not in their correct position
	eg: 
	[ 1 3 5
	  0 4 7
	  6	2 8 ] 

	  would have tile mismatch heuristic of 5, becaue [1,3,5,7,2] are not in their correct positions
*/

int tileMismatch(puzzleState p){
	int mismatches=0;	
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(p.positions[i][j]!=0)
				if(i!=actualRow(p.positions[i][j],3) || j!=actualColumn(p.positions[i][j],3))
					mismatches++;
		}
	}
	return mismatches;
} 

/*
	Helper structures used to define a priority queue sorted in order of f(n) = g(n) + h(n)
	where g(n) is the cost of reaching that state (no of steps to reach that state)
	and h(n) is the heuristic
*/

struct compareManhattan{
	bool operator()(puzzleState const& p1, puzzleState const& p2){
		return (p1.depth + manhattanDistance(p1)) > (p2.depth + manhattanDistance(p2));
		//sorts in ascending order
	}
};

struct compareTileMismatch{
	bool operator()(puzzleState const& p1, puzzleState const& p2){
		return (p1.depth + tileMismatch(p1)) > (p2.depth + tileMismatch(p2));
	}
};

/*
	Function to test if a given state is the the goal state:
	[0  1  2
	 3  4  5
	 6  7  8]
*/

bool goalTest(puzzleState p){
	if( p.positions[0][0] == 0 && p.positions[0][1] == 1 && p.positions[0][2] == 2 &&
		p.positions[1][0] == 3 && p.positions[1][1] == 4 && p.positions[1][2] == 5 &&
		p.positions[2][0] == 6 && p.positions[2][1] == 7 && p.positions[2][2] == 8 )
		return true;
	else
		return false;
}


/*
	Function to see if 2 puzzleStates have the same configuration of board
	Returns true if same config, else false.
*/

bool checkEqualPositions(puzzleState p1, puzzleState p2){
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(p1.positions[i][j]!=p2.positions[i][j])
				return false;
		}
	}
	return true;
}

/*
	Checks if a particular node check has already been visited or not
	if it has, returns true. Else false.
*/
bool isInPreviousQueue(vector<puzzleState> prev, puzzleState check){
	puzzleState curr;
	if(prev.size()==0)
		return false;
	for(auto it=prev.begin();it!=prev.end();it++){
		curr = (*it);
		if(checkEqualPositions(curr,check))
			return true;
	}
	return false;
}

/*
	Function to expand a given node in a normal BFS manner.
	It also makes sure that we don't expand to children that have already been visited.
	i.e Prevent redundant nodes from appearing
	Returns the children of a node
	Checks in all 4 directions (up,down,left,right), and moves the 0 in those directions if possible to generate new children which
	have not been previously visited
*/
vector<puzzleState> expand(puzzleState C, vector<puzzleState> prev){
	//try all four combinations(up,down,left,right)
	//add only valid ones into the queue, and only if these states are not already present
	//depth is stored in the new puzzleState as C.depth + 1
	puzzleState temp;
	vector<puzzleState> expansion;
	int row;
	int col;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(C.positions[i][j]==0){
				row=i;
				col=j;
			}
		}
	}
	//if going Up is possible
	if(row>0)
	{
		int swap;
		temp = C;
		swap = temp.positions[row-1][col];
		temp.positions[row-1][col]=temp.positions[row][col];
		temp.positions[row][col] = swap;
		temp.depth = C.depth + 1; 
		//child has greater depth
		//insert child only if it is not already in the queue, IE if it has not already been visited
		if(!isInPreviousQueue(prev,temp))
			expansion.push_back(temp);
	}
	//if doing down is possible
	if(row<2)
	{
		int swap;
		temp = C;
		swap = temp.positions[row+1][col];
		temp.positions[row+1][col]=temp.positions[row][col];
		temp.positions[row][col] = swap;
		temp.depth = C.depth + 1; 
		//child has greater depth
		//insert child only if it is not already in the queue, IE if it has not already been visited
		if(!isInPreviousQueue(prev,temp))
			expansion.push_back(temp);
	}
	//if going left is possible
	if(col>0)
	{
		int swap;
		temp = C;
		swap = temp.positions[row][col-1];
		temp.positions[row][col-1]=temp.positions[row][col];
		temp.positions[row][col] = swap;
		temp.depth = C.depth + 1; 
		//child has greater depth
		//insert child only if it is not already in the queue, IE if it has not already been visited
		if(!isInPreviousQueue(prev,temp))
			expansion.push_back(temp);
	}
	//if going right is possible
	if(col<2)
	{
		int swap;
		temp = C;
		swap = temp.positions[row][col+1];
		temp.positions[row][col+1] = temp.positions[row][col];
		temp.positions[row][col] = swap;
		temp.depth = C.depth + 1;
		//child has greater depth
		//insert child only if it is not already in the queue, IE if it has not already been visited
		if(!isInPreviousQueue(prev,temp))
			expansion.push_back(temp);
	}
	return expansion;
}

/*
	Helper function to append the contents of a vector of puzzleStates temp3 to temp1.
*/

vector<puzzleState> append(vector<puzzleState> temp1, vector<puzzleState> temp3){
	puzzleState next;
	for(auto it = temp3.begin();it!=temp3.end();it++){
		next = (*it);
		temp1.push_back(next);
	}
	return temp1;
}

/*
	This function expands a node using depth-limited BFS. It uses expand() as a helper function to expand one node.
	It expands all nodes from the parent till depth d, then adds them to a vector
	In ModifiedAStar, this vector is converted to a priority queue sorted by the cost function f(n)
	At each stage, the node with smallest cost is taken next and checked to see if it is a goal
	We also make sure no redundant nodes are visited (covered in expand() function)
*/

vector<puzzleState> modifiedExpand(puzzleState S, int depth, int &nodesExpanded, vector<puzzleState> &allnodes){
	//temp1 can be treated as a queue of all the states expanded
	vector<puzzleState> temp1;
	temp1 = expand(S,allnodes);
	if(temp1.size()==0)
	{
		return temp1;
	}
	nodesExpanded++;
	allnodes = append(allnodes, temp1);
	
	vector<puzzleState> temp2 = temp1;
	vector<puzzleState> temp3;
	puzzleState C;
	int d = depth-1;
	while (d>0){
		for(auto it = temp2.begin();it!=temp2.end();it++){
			C = (*it);
			//temp3 stores newly discovered expanded nodes
			temp3 = expand(C, allnodes);
			if(temp3.size()==0)
			{

			}
			else
			{
				allnodes = append(allnodes, temp3);
				nodesExpanded++;
				//temp1 stores all nodes that have been visited by the depth limited BFS
				temp1 = append(temp1, temp3);
			}
			
		}
		d--;
		temp2 = temp3;
	}
	return temp1;
}





/*	
	This function accepts an initialState, a heuristic, and a depth.
	At each stage, we check to see if a node is a goal state.
	If it is, we return it.
	Otherwise, we generate all of its descendants till depth bfsDepth using modifiedExpand, then add those nodes into a priority queue
	sorted by f(n) = g(n) + h(n)
	At each stage, the next node taken is the node with least cost.
	This function also computes the total number of nodes we expand, and the largest size of the fringe at any point during the algorithm
	'm' represents Manhattan Distance
	't' represents Tile Mismatches
*/
pair<pair<int,int>,puzzleState> modifiedAStar(puzzleState initialState, char heuristic, int bfsDepth){
	int maxFringeSize=1;
	int nodesExpanded=0;
	puzzleState S;
	if(heuristic=='m'){
		//fringe is a queue sorted in increasing order of f(n) = depth + h(n)
		priority_queue<puzzleState,vector<puzzleState>,compareManhattan> fringe;
		vector<puzzleState> temp;
		vector<puzzleState> allnodes;
		allnodes.push_back(initialState);
		puzzleState T;
		S = initialState;
		while(!goalTest(S)){
			temp = modifiedExpand(S,bfsDepth,nodesExpanded, allnodes);
			for(auto it = temp.begin(); it!=temp.end(); it++){
				T=(*it);
				fringe.push(T);
				//fringe stores the states in ascending order of f(n)
				//c gets the heuristic h(n), we then compute f(n) internally as h(n) + g(n), where g(n) is the number of steps till that point, ie depth
				//depth is stored within the state itself.
			}
			if(fringe.size() > maxFringeSize)
				maxFringeSize = fringe.size();
			if(fringe.size()!=0)
			{
				S = fringe.top();
				fringe.pop();
			}
			else
				break;
			
		}
	}
	else{
		priority_queue<puzzleState,vector<puzzleState>,compareTileMismatch> fringe;
		vector<puzzleState> temp;
		vector<puzzleState> allnodes;
		allnodes.push_back(initialState);
		puzzleState T;
		S = initialState;
		while(!goalTest(S)){
			temp = modifiedExpand(S,bfsDepth,nodesExpanded, allnodes);	
			for(auto it = temp.begin(); it!=temp.end(); it++){
				T=(*it);
				fringe.push(T);
				//c gets the heuristic h(n), we then compute f(n) internally as h(n) + g(n), where g(n) is the number of steps till that point, ie depth
				//depth is stored within the state itself.
			}
			if(fringe.size()>maxFringeSize)
				maxFringeSize = fringe.size();
			S = fringe.top();
			fringe.pop();
		}
	}
	//values are maxFringeSize, no of nodes generated(nodesExpanded), and final state(S)
	pair<pair<int,int>,puzzleState> result;
	pair<int,int> info;
	info.first = maxFringeSize;
	info.second = nodesExpanded;
	result = make_pair(info,S);
	return result; 
}

bool checkSolvable(puzzleState p){
	int inversions = 0;
	int row1,row2,col1,col2;

    for(int i = 0; i < 9; ++i) {
        for(int j = i+1; j < 9; ++j) {
            
            row1 = actualRow(i,3);
            col1 = actualColumn(i,3);

            row2 = actualRow(j,3);
            col2 = actualColumn(j,3);
            //Dont include the tile with 0 in inversions
            if( p.positions[row1][col1] != 0 && 
            	p.positions[row2][col2] != 0 && 
            	p.positions[row1][col1] > p.positions[row2][col2])
                inversions++;
        }
    }
    if(inversions%2)
    	return false;
    else
    	return true;
}

void findZero(puzzleState p, int &row, int& col){
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(p.positions[i][j]==0){
				row=i;
				col=j;
				return ;
			}
		}
	}
}

/*
	This function generates a random solvable board by backtracking an arbitrary amount of times from the goal state.
*/
puzzleState generateRandomSolvablePuzzle(){
	puzzleState start;
	start.positions[0][0] =0;
	start.positions[0][1] =1;
	start.positions[0][2] =2;
	start.positions[1][0] =3;
	start.positions[1][1] =4;
	start.positions[1][2] =5;
	start.positions[2][0] =6;
	start.positions[2][1] =7;
	start.positions[2][2] =8;
	start.depth=0;
	int swaps = rand()%80;
	int row,col;
	int direction;
	for(int i=0;i<swaps;i++){
		findZero(start, row, col);
		direction = 1 + (rand() % 4);
		//move up if possible, otherwise retry
		if(direction == 1){
			if(row>0){	
				start.positions[row][col]=start.positions[row-1][col];
				start.positions[row-1][col]=0;	
				row= row-1;
			}
			else{
				i--;
				continue;
			}
		}
		//move down if possible, otherwise retry
		else if(direction == 2){
			if(row<2){	
				start.positions[row][col]=start.positions[row+1][col];
				start.positions[row+1][col]=0;	
				row = row+1;
			}
			else{
				i--;
				continue;
			}
		}
		//move left if possible, otherwise retry
		else if(direction == 3){
			if(col>0){	
				start.positions[row][col]=start.positions[row][col-1];
				start.positions[row][col-1]=0;	
				col = col-1;
			}
			else{
				i--;
				continue;
			}
		}
		//move right if possible, otherwise retry
		else if(direction == 4){
			if(col<2){	
				start.positions[row][col]=start.positions[row][col+1];
				start.positions[row][col+1]=0;
				col = col+1;	
			}
			else{
				i--;
				continue;
			}
		}
	}
	return start;
}

/*
	Helper function to check if a state is a goal State
*/

string checkOptimal(puzzleState p){
	if(goalTest(p))
		return "YES";
	else
		return "NO";
}

/*
	Helper function for displaying the following:
	Heuristic 	BfsDepth 	is Solution Optimal?	Max Fringe Size  	No of nodes expanded 	
*/
void displayCost(puzzleState initialState, pair<pair<int,int>,puzzleState> result, char heuristic, int depth){
	//Heuristic
	if(heuristic=='m')
		cout<<"\nManhattan Dist\t\t";
	else if(heuristic=='t')
		cout<<"\nTile Mismatch \t\t";
	//Depth used
	cout<<depth<<"         \t";
	//isOptimal
	cout<<checkOptimal(result.second)<<"              \t";
	//Max Fringe Size
	cout<<result.first.first<<"                \t";
	//No of nodes generated
	cout<<result.first.second<<"		\t";
	//How deep is the solution
	cout<<result.second.depth;

}
/*
puzzleState createTest(){
	puzzleState initialState;
	initialState.positions[0][0]=1;
	initialState.positions[0][1]=2;
	initialState.positions[0][2]=0;
	initialState.positions[1][0]=3;
	initialState.positions[1][1]=4;
	initialState.positions[1][2]=5;
	initialState.positions[2][0]=6;
	initialState.positions[2][1]=7;
	initialState.positions[2][2]=8;
	initialState.depth=0;
	return initialState;

}
*/

int main()
{
	int depth;
	cout<<"Enter the depth of depth limiting BFS: ";
	cin>>depth;	
	//cout<<"ASDF";
	puzzleState initialState;
	srand(time(NULL));
	//Values are maxFringeSize, no of nodes generated, and final state
	pair<pair<int,int>,puzzleState> result1;
	pair<pair<int,int>,puzzleState> result2;
	pair<pair<int,int>,puzzleState> result3;
	pair<pair<int,int>,puzzleState> result4;

	for(int i=0;i<10;i++){
		initialState = generateRandomSolvablePuzzle();
		if(!checkSolvable(initialState))
		{
			cout<<"initial state not valid.";
			continue;
		}

		result1 = modifiedAStar(initialState, 'm', 1); 
		result2 = modifiedAStar(initialState, 'm', depth);
		result3 = modifiedAStar(initialState, 't', 1);
		result4 = modifiedAStar(initialState, 't', depth);

		cout<<"\n\n\n\nIteration "<<i<<":";
		cout<<"\nStart State is :\n";
		displayState(initialState);
		cout<<"\n\nHeuristic used\t BFSDepth used\t Solution Optimal?\t Max Fringe Size\t Nodes Expanded\t Depth of Solution\n";
		displayCost(initialState,result1,'m',1);
		displayCost(initialState,result2,'m',depth);	
		displayCost(initialState,result3,'t',1);
		displayCost(initialState,result4,'t',depth);
		cout<<"\n\n\n\n\n\n";
	}

	cout<<"\nFrom this, we can conclude that Manhattan distance is a better heuristic than tile mismatches.\n\n";
	cout<<"\nNote: Nodes expanded is the number of nodes whose children we added to the list. It is not the number of nodes in the graph.\n";
	cout<<"Total number of nodes in the graph can easily be calculated, by checking the size of the allnodes vector.\n\n";
	return 0;
}

