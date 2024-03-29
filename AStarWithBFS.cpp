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
	What is left:
	Finish modifiedexpand and modifiedAstar
	generate random initial cases
	check them to see if they are solvable(even number of inversions) before generating them
	compare results for all 4 calls, for 10 different initial states
*/
#include <iostream>
#include <vector>
#include <queue>
#include <ctime>


using namespace std;

struct puzzleState{
	int positions[3][3]; 
	int depth;
	//depth from starting state
};

int actualRow(int num,int totalColumns){
	return num / totalColumns;
}

int actualColumn(int num, int totalColumns){
	return num % totalColumns;
}

int manhattanDistance(puzzleState p){
	int distance=0;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(p.positions[i][j]!=0)
				distance += i - actualRow(p.positions[i][j],3) + j - actualColumn(p.positions[i][j],3);
		}
	}
	return distance;
}

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

bool goalTest(puzzleState p){
	if( p.positions[0][0] == 0 && p.positions[0][1] == 1 && p.positions[0][2] == 3 &&
		p.positions[1][0] == 4 && p.positions[1][1] == 5 && p.positions[1][2] == 6 &&
		p.positions[2][0] == 7 && p.positions[2][1] == 8 && p.positions[2][2] == 9 )
		return true;
	else
		return false;
}


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
bool checkEqualPositions(puzzleState p1, puzzleState p2){
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(p1.positions[i][j]!=p2.positions[i][j])
				return false;
		}
	}
	return true;
}

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

vector<puzzleState> append(vector<puzzleState> temp1, vector<puzzleState> temp3){
	puzzleState next;
	for(auto it = temp3.begin();it!=temp3.end();it++){
		next = (*it);
		temp1.push_back(next);
	}
	return temp1;
}

vector<puzzleState> modifiedExpand(puzzleState S, int depth){
	//temp1 can be treated as a queue of all the states expanded
	vector<puzzleState> temp1;
	temp1 = expand(S,temp1);
	vector<puzzleState> temp2 = temp1;
	vector<puzzleState> temp3;
	puzzleState C;
	int d = depth-1;
	while (d>0){
		for(auto it = temp2.begin();it!=temp2.end();it++){
			C = (*it);
			//temp3 stores newly discovered expanded nodes
			temp3 = expand(C, temp1);
			//temp1 stores all nodes that have been visited by the depth limited BFS
			temp1 = append(temp1, temp3);
		}
		d--;
		temp2 = temp3;
	}
	return temp1;
}


//here, the character represents the heurisitic that is used
//m for manhattan distance
//t for tile_mismatch
//returns max size of fringe and no of nodes generated

//might have to write separate algorithms for 2 heuristic cases. Could be done easily with a priority queue


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


pair<pair<int,int>,puzzleState> modifiedAStar(puzzleState initialState, char heuristic, int bfsDepth){
	int maxFringeSize=0;
	int nodesExpanded=0;
	puzzleState S;
	if(heuristic=='m'){
		//fringe is a queue sorted in increasing order of f(n) = depth + h(n)
		priority_queue<puzzleState,vector<puzzleState>,compareManhattan> fringe;
		vector<puzzleState> temp;
		puzzleState T;
		S = initialState;
		while(!goalTest(S)){
			nodesExpanded++;
			temp = modifiedExpand(S,bfsDepth);
			if(temp.size()>maxFringeSize)
				maxFringeSize = temp.size();
			for(auto it = temp.begin(); it!=temp.end(); it++){
				T=(*it);
				fringe.push(T);
				//fringe stores the states in ascending order of f(n)
				//c gets the heuristic h(n), we then compute f(n) internally as h(n) + g(n), where g(n) is the number of steps till that point, ie depth
				//depth is stored within the state itself.
			}
			S = fringe.top();
			fringe.pop();
		}
	}
	else{
		priority_queue<puzzleState,vector<puzzleState>,compareTileMismatch> fringe;
		vector<puzzleState> temp;
		puzzleState T;
		S = initialState;
		while(!goalTest(S)){
			nodesExpanded++;
			temp = modifiedExpand(S,bfsDepth);
			if(temp.size()>maxFringeSize)
				maxFringeSize = temp.size();	
			for(auto it = temp.begin(); it!=temp.end(); it++){
				T=(*it);
				fringe.push(T);
				//c gets the heuristic h(n), we then compute f(n) internally as h(n) + g(n), where g(n) is the number of steps till that point, ie depth
				//depth is stored within the state itself.
			}
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


bool isPresent(puzzleState start, int tile){
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(start.positions[i][j]==tile)
				return true;
		}
	}
	return false;
}

bool checkSolvable(puzzleState p){
	int inversions = 0;
	int row1,row2,col1,col2;

    for(int i = 0; i < 9; ++i) {
        for(int j = i+1; j < 9; ++j) {
            //Dont include the tile with 0
            row1 = actualRow(i,3);
            col1 = actualColumn(i,3);

            row2 = actualRow(j,3);
            col2 = actualColumn(j,3);

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

//we generate an 8 puzzle by randomly adding new tiles to an empty board
puzzleState generateRandomSolvablePuzzle(){
	puzzleState start;
	start.positions[0][0] =-1;
	start.positions[0][1] =-1;
	start.positions[0][2] =-1;
	start.positions[1][0] =-1;
	start.positions[1][1] =-1;
	start.positions[1][2] =-1;
	start.positions[2][0] =-1;
	start.positions[2][1] =-1;
	start.positions[2][2] =-1;
	start.depth=0;
	bool solvable = false;
	int tile;
	cout<<"Generating puzzle";
	while(!solvable){
		for(int i=0; i < 9;i++){
			tile = rand()%9;
			//find a unique tile which has not already been inserted
			while(isPresent(start,tile)){
				tile = rand() % 9;
			}
			start.positions[actualRow(i,3)][actualColumn(i,3)] = tile;
		}
		solvable = checkSolvable(start);
	}
	cout<<"Generated puzzle";
	return start;
}

/*

	In this program we use 0 to represent a blank tile
	Goal state : 	[0  1  2
					 3	4  5
					 6  7  8]
	Initial state is a random solvable state
	A state is said to be solvable if it has an even number of inversions
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

string checkOptimal(puzzleState p){
	if(goalTest(p))
		return "YES";
	else
		return "NO";
}

void displayCost(puzzleState initialState, pair<pair<int,int>,puzzleState> result, char heuristic, int depth){
	//Heuristic
	if(heuristic=='m')
		cout<<"\nManhattan Dist\t";
	else if(heuristic=='t')
		cout<<"\nTile Mismatch \t";
	//Depth used
	cout<<depth<<"          \t";
	//isOptimal
	cout<<checkOptimal(result.second)<<"              \t";
	//Max Fringe Size
	cout<<result.first.first<<"                \t";
	//No of nodes generated
	cout<<result.first.second;


}

int main(int argc, char* argv[])
{
	int depth=2;	
	cout<<"Enter the depth value:";
	cin>>depth;
	cout<<"AS";
	puzzleState initialState;
	//Values are maxFringeSize, no of nodes generated, and final state
	pair<pair<int,int>,puzzleState> result1;
	pair<pair<int,int>,puzzleState> result2;
	pair<pair<int,int>,puzzleState> result3;
	pair<pair<int,int>,puzzleState> result4;

	for(int i=0;i<10;i++){
		initialState = generateRandomSolvablePuzzle();
		//'m' represents manhattan distance as heuristic
		//'t' represents tile mismatches as heuristic
		result1 = modifiedAStar(initialState, 'm', 1); 
		result2 = modifiedAStar(initialState, 'm', depth);
		result3 = modifiedAStar(initialState, 't', 1);
		result4 = modifiedAStar(initialState, 't', depth);
		cout<<"\n\n\n\nIteration"<<i<<"";
		cout<<"\nThe initial State is :\n";
		displayState(initialState);
		cout<<"\n\nHeuristic used\t Depth used\t Solution Optimal?\t Max Fringe Size\t Nodes Generated\n";
		displayCost(initialState,result1,'m',1);
		displayCost(initialState,result2,'m',depth);	
		displayCost(initialState,result3,'t',1);
		displayCost(initialState,result4,'t',depth);
	}
	return 0;
}

/*
	Heuristic used	 Depth used	 Solution Optimal?	 Max Fringe size 	Nodes Generated
	Manhattan
*/
