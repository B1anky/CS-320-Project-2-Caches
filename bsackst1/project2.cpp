//Name: Brett Sackstein
//BU-ID: bsackst1
//B-Number: 00458114
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <math.h>

using namespace std;

struct instruction{
	unsigned long long address;
	string op;
    	instruction(unsigned long long addr, string opIn): address(addr), op(opIn){}
    	instruction(){}
};

vector<instruction> parseInput(string file);
int getVictimInd1D(long long traceSize, int ways, vector<int> LRU);
int getVictimInd2D(long long traceSize, int ways, vector<vector<int>> LRU, unsigned long long blockInd);
vector<unsigned long long> parseAddress(instruction instr, int size);
unsigned long long parseAddress2(instruction instr, int bitLength);
vector<unsigned long long> parseAddress3(instruction instr, int size, int prefetchOffset);
vector<int> directMappedCache(vector<instruction> trace, int bitLength);
vector<int> setAssociativeCache(vector<instruction> trace, int bitLength, int ways);
vector<int> fullyAssociativeCacheLRU(vector<instruction> trace, int bitLength, int ways);
vector<int> fullyAssociativeCacheHotCold(vector<instruction> trace, int bitLength, int ways);
vector<int> noAllocOnWriteMissSetAssociativeCache(vector<instruction> trace, int bitLength, int ways);
vector<int> setAssociativeCachePrefetching(vector<instruction> trace, int bitLength, int ways);
vector<int> setAssociativeCachePrefetchingOnMiss(vector<instruction> trace, int bitLength, int ways);
void printResults(vector<vector<vector<int>>> resultVec, const char *argv);

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cout << "Usage: ./cache-sim [input trace] [output file]\n";
		return 1; //error
	}

	vector<instruction> trace = parseInput(argv[1]);
	//int args refer to 2^args where 2^10 = 1KB, 2^12 = 4KB, 2^14 = 16KB, 2^15 = 32KB
	//the args are divided by line size which is 2^args/2^5
	vector<vector<int>> resultVec1;
	vector<vector<int>> resultVec2;
	vector<vector<int>> resultVec3;
	vector<vector<int>> resultVec4;
	vector<vector<int>> resultVec5;
	vector<vector<int>> resultVec6;
	vector<vector<int>> resultVec7;
	vector<vector<vector<int>>> finalResult;

	//Part 1)
	vector<int> result1 = directMappedCache(trace, 5);
	vector<int> result2 = directMappedCache(trace, 7);
	vector<int> result3 = directMappedCache(trace, 9);
	vector<int> result4 = directMappedCache(trace, 10);
	resultVec1.push_back(result1);
	resultVec1.push_back(result2);
	resultVec1.push_back(result3);
	resultVec1.push_back(result4);

	//Part 2)
	vector<int> result5 = setAssociativeCache(trace, 9, 2);
	vector<int> result6 = setAssociativeCache(trace, 9, 4);
	vector<int> result7 = setAssociativeCache(trace, 9, 8);
	vector<int> result8 = setAssociativeCache(trace, 9, 16);
	resultVec2.push_back(result5);
	resultVec2.push_back(result6);
	resultVec2.push_back(result7);
	resultVec2.push_back(result8);

	//Part 3 LRU)
	vector<int> result9 = fullyAssociativeCacheLRU(trace, 9, 512);//essentially a 512 way directMapped cache with LRU
	resultVec3.push_back(result9);

	//Part 3 Hot and Cold Replacement)
	vector<int> result10 = fullyAssociativeCacheHotCold(trace, 9, 512);
	resultVec4.push_back(result10);

	//Part 4)
	vector<int> result11 = noAllocOnWriteMissSetAssociativeCache(trace, 9, 2);
	vector<int> result12 = noAllocOnWriteMissSetAssociativeCache(trace, 9, 4);
	vector<int> result13 = noAllocOnWriteMissSetAssociativeCache(trace, 9, 8);
	vector<int> result14 = noAllocOnWriteMissSetAssociativeCache(trace, 9, 16);
	resultVec5.push_back(result11);
	resultVec5.push_back(result12);
	resultVec5.push_back(result13);
	resultVec5.push_back(result14);

	//Part 5)
	vector<int> result15 = setAssociativeCachePrefetching(trace, 9, 2);
	vector<int> result16 = setAssociativeCachePrefetching(trace, 9, 4);
	vector<int> result17 = setAssociativeCachePrefetching(trace, 9, 8);
	vector<int> result18 = setAssociativeCachePrefetching(trace, 9, 16);
	resultVec6.push_back(result15);
	resultVec6.push_back(result16);
	resultVec6.push_back(result17);
	resultVec6.push_back(result18);

	//Part 6
	vector<int> result19 = setAssociativeCachePrefetchingOnMiss(trace, 9, 2);
	vector<int> result20 = setAssociativeCachePrefetchingOnMiss(trace, 9, 4);
	vector<int> result21 = setAssociativeCachePrefetchingOnMiss(trace, 9, 8);
	vector<int> result22 = setAssociativeCachePrefetchingOnMiss(trace, 9, 16);
	resultVec7.push_back(result19);
	resultVec7.push_back(result20);
	resultVec7.push_back(result21);
	resultVec7.push_back(result22);

	finalResult.push_back(resultVec1);
	finalResult.push_back(resultVec2);
	finalResult.push_back(resultVec3);
	finalResult.push_back(resultVec4);
	finalResult.push_back(resultVec5);
	finalResult.push_back(resultVec6);
	finalResult.push_back(resultVec7);

	printResults(finalResult, argv[2]);

	return 0;
}

//Builds a vector of all instructions
vector<instruction> parseInput(string file){
	vector<struct instruction> table;
	unsigned long long addr;
	string behavior;
	ifstream infile(file);
	while(infile >> behavior >> std::hex >> addr) {
		struct instruction instr;
		instr.op = behavior;
		instr.address = addr;
		table.push_back(instr);
	}
	return table;
}

//Returns a vector containing the address parsed into the cache index and tag value
vector<unsigned long long> parseAddress(instruction instr, int bitLength){
	vector<unsigned long long> arr;
	arr.push_back((instr.address >> 5) % (int)pow(2, bitLength)); //gets block index
	arr.push_back(instr.address / pow(2, bitLength));//gets tagValue to compare at block index
	return arr;
}

vector<unsigned long long> parseAddress3(instruction instr, int bitLength, int prefetchOffset){
	vector<unsigned long long> arr;
	arr.push_back(((instr.address + prefetchOffset) >> 5) % (int)pow(2, bitLength)); //gets block index
	arr.push_back((instr.address + prefetchOffset) / pow(2, bitLength));//gets tagValue to compare at block index
	return arr;
}

//Returns a vector containing the address parsed into the cache index and tag value
unsigned long long parseAddress2(instruction instr, int bitLength){
	return (unsigned long long) (instr.address / pow(2, bitLength - 4));//gets tagValue to compare at block index
}

//The direct-mapped cache function will make a single way table of size 2^bitLength
vector<int> directMappedCache(vector<instruction> trace, int bitLength){
	int cacheSize = pow(2, bitLength);
	vector<unsigned long long> tagTable(cacheSize, 0); //set up initial tag table with 0s
	//now loop through trace and parse for tag and whether S or L
	int hits = 0;
	for(unsigned int i =0; i < trace.size(); i++){
		unsigned long long blockInd = parseAddress(trace[i], bitLength)[0];
		unsigned long long tag = parseAddress(trace[i], bitLength)[1];

		//either store in table or load from table, if tag exists -> a hit else miss
		if(tagTable[blockInd] == tag){
			hits++;
		}else{ //Otherwise we need to store in table if we had a miss 
			tagTable[blockInd] = tag;
		}
	}
	vector<int> results;
	results.push_back(hits);
	results.push_back(trace.size());
	return results;
}

//The set associative cache function will make a ways way table of size 2^9 (16 KB) with LRU replacement
vector<int> setAssociativeCache(vector<instruction> trace, int bitLength, int ways){
	int cacheSize = (int)(pow(2, bitLength - log2(ways)));
	//set up initial tag table with 0s and the LRU table with -1s
	vector<unsigned long long> tagTable(cacheSize, 0);
	vector<vector<unsigned long long>> setAssocCache(ways, tagTable);
	vector<int> LRUTable(cacheSize, -1);
	vector<vector<int>> LRU(ways, LRUTable);
	//now loop through trace and parse for tag and whether S or L
	int hits = 0;
	for(unsigned int i = 0; i < trace.size(); i++){
		unsigned long long blockInd = parseAddress(trace[i], bitLength - log2(ways))[0];
		unsigned long long tag = parseAddress(trace[i], bitLength)[1];

		//either store in table or load from table, if tag exists -> a hit else miss
		bool done1 = false;
		bool done2 = false;
		for(int j = 0; j < ways; j++){
			if(setAssocCache[j][blockInd] == tag){
				hits++;
				LRU[j][blockInd] = i; //update LRU
				done1 = true;
				done2 = true;
				break; //have to break to not double count
			}
		}

		if(done1 == false){
			//Otherwise we need to store in block if we had a miss
			//look for empty spot, if not implement LRU
			for(int x = 0; x < ways; x++){
				if(LRU[x][blockInd] == -1){//implies empty spots open no need for LRU yet
					setAssocCache[x][blockInd] = tag;
					LRU[x][blockInd] = i; //update LRU
					done2 = true;
					break;
				}
			}
		}

		if(done2 == false){//implement LRU
			int victimInd = getVictimInd2D(trace.size(), ways, LRU, blockInd);
			setAssocCache[victimInd][blockInd] = tag;//evict the victimInd by replacing with new tag
			LRU[victimInd][blockInd] = i; //reset the evicted index LRU to current time
		}
	}
	vector<int> results;
	results.push_back(hits);
	results.push_back(trace.size());
	return results;
}

vector<int> fullyAssociativeCacheLRU(vector<instruction> trace, int bitLength, int ways){
	int cacheSize = ways;
	vector<unsigned long long> tagTable(cacheSize, 0); //set up initial tag table with 0s
	vector<int> LRU(cacheSize, -1);
	//now loop through trace and parse for tag and whether S or L
	int hits = 0;
	int indexInTable = 0;
	for(unsigned int i = 0; i < trace.size(); i++){
		unsigned long long tag = parseAddress2(trace[i], bitLength);
		bool hit = false;
		//check if it's in the table
		for(int j = 0; j < cacheSize; j++){
			if(tagTable[j] == tag){
				hits++;
				LRU[j] = i;
				hit = true;
				break;
			}
		}

		if(!hit){
			//table not full, just push back
			if(indexInTable < cacheSize){
				tagTable[indexInTable] = tag;
				LRU[indexInTable] = i;
				indexInTable++;
			}else{
				int victimInd = getVictimInd1D(trace.size(), ways, LRU);
				tagTable[victimInd] = tag;//evict the victimInd by replacing with new tag
				LRU[victimInd] = i; //reset the evicted index LRU to current time
			}
		}
	}
	vector<int> results;
	results.push_back(hits);
	results.push_back(trace.size());
	return results;
}

vector<int> fullyAssociativeCacheHotCold(vector<instruction> trace, int bitLength, int ways){
	int cacheSize = ways;
	vector<unsigned long long> FACache(cacheSize, 0); //set up initial tag table with 0s
	vector<int> hotAndColdArr(cacheSize - 1, 0);

	int hits = 0;
	for(unsigned int i = 0; i < trace.size(); i++){
		unsigned long long tag = parseAddress2(trace[i], bitLength);
		int ind;
		bool hit = false;

		//Search through whole cache and look for hit
		for(int j = 0; j < cacheSize; j++){
			if(FACache[j] == tag){
				ind = j;
				hit = true;
				hits++;
				break; //break to stop double counting
			}	
		}

		int lower = 0;
		int upper = cacheSize - 1; //The upper index is always the cache size less one
		int position = (upper + lower)/2; //start at the middle, like the root of a tree
		
		if(hit){//if we have a hit we need to update the Hot and Cold bits
			while((ind/2)*2 != position){//index hit at
				if((ind/2)*2 < position){//current index is below position -> position in the new upper bound
					upper = position;
					hotAndColdArr[position] = 0; //Set the current "upper" position to cold
				}else{//current index is above position -> position in the new lower bound
					lower = position;
					hotAndColdArr[position] = 1;// Set the current "lower" position to hot
				}
				position = (upper + lower)/2;//recalculate the new "root" index until ind = position
			}
			hotAndColdArr[position] = ind - position;
			FACache[ind] = tag;
		}else{
			while(position % 2 == 1){
				if(hotAndColdArr[position] == 1){
					upper = position;
					hotAndColdArr[position] = 0;
				}else{
					lower = position;
					hotAndColdArr[position] = 1;
				}
				position=(upper + lower)/2;
			}

			if(hotAndColdArr[position] == 1){
				hotAndColdArr[position] = 0;
				FACache[position] = tag;
			}else{
				hotAndColdArr[position] = 1;
				FACache[position + 1] = tag;
			}   
		}
	}
    
	vector<int> results;
	results.push_back(hits);
	results.push_back(trace.size());
	return results;
}

//The set associative cache function will make a ways way table of size 2^9 (16 KB) with LRU replacement
vector<int> noAllocOnWriteMissSetAssociativeCache(vector<instruction> trace, int bitLength, int ways){
	int cacheSize = (int)(pow(2, bitLength - log2(ways)));
	//set up initial tag table with 0s and the LRU table with -1s
	vector<unsigned long long> tagTable(cacheSize, 0);
	vector<vector<unsigned long long>> setAssocCache(ways, tagTable);
	vector<int> LRUTable(cacheSize, -1);
	vector<vector<int>> LRU(ways, LRUTable);
	//now loop through trace and parse for tag and whether S or L
	int hits = 0;
	for(unsigned int i = 0; i < trace.size(); i++){
		unsigned long long blockInd = parseAddress(trace[i], bitLength - log2(ways))[0];
		unsigned long long tag = parseAddress(trace[i], bitLength)[1];

		//either store in table or load from table, if tag exists -> a hit else miss
		bool done1 = false;
		bool done2 = false;
		for(int j = 0; j < ways; j++){
			if(setAssocCache[j][blockInd] == tag){
				hits++;
				LRU[j][blockInd] = i; //update LRU
				done1 = true;
				done2 = true;
				break; //have to break to not double count
			}
		}

		if(done1 == false  && trace[i].op == "L"){
			//Otherwise we need to store in block if we had a miss
			//look for empty spot, if not implement LRU
			for(int x = 0; x < ways; x++){
				if(LRU[x][blockInd] == -1){//implies empty spots open no need for LRU yet
					setAssocCache[x][blockInd] = tag;
					LRU[x][blockInd] = i; //update LRU
					done2 = true;
					break;
				}
			}
		}

		if(done2 == false && trace[i].op == "L"){//implement LRU
			int victimInd = getVictimInd2D(trace.size(), ways, LRU, blockInd);
			setAssocCache[victimInd][blockInd] = tag;//evict the victimInd by replacing with new tag
			LRU[victimInd][blockInd] = i; //reset the evicted index LRU to current time
		}
	}
	vector<int> results;
	results.push_back(hits);
	results.push_back(trace.size());
	return results;
}

vector<int> setAssociativeCachePrefetching(vector<instruction> trace, int bitLength, int ways){
	unsigned int cacheSize = (int)(pow(2, bitLength - log2(ways)));
	//set up initial tag table with 0s and the LRU table with -1s
	vector<unsigned long long> tagTable(cacheSize, 0);
	vector<vector<unsigned long long>> setAssocCache(ways, tagTable);
	vector<int> LRUTable(cacheSize, -1);
	vector<vector<int>> LRU(ways, LRUTable);
	//now loop through trace and parse for tag and whether S or L
	int hits = 0;
	int totalFetches = 0;
	for(unsigned int i = 0; i < trace.size(); i++){
		unsigned long long blockInd = parseAddress(trace[i], bitLength - log2(ways))[0];
		unsigned long long tag = parseAddress(trace[i], bitLength)[1];
		unsigned long long nextInd = parseAddress3(trace[i], bitLength - log2(ways), 32)[0];
		unsigned long long nextTag = parseAddress3(trace[i], bitLength, 32)[1];

		bool isHit = false;

		for(int j = 0; j < ways; j++){
			if(setAssocCache[j][blockInd] == tag){
				hits++;
				LRU[j][blockInd] = totalFetches; //update LRU
				isHit = true;
				break; //have to break to not double count
			}
		}

		if(!isHit){//implement LRU
			int victimInd = getVictimInd2D(trace.size(), ways, LRU, blockInd);
			setAssocCache[victimInd][blockInd] = tag;//evict the victimInd by replacing with new tag
			LRU[victimInd][blockInd] = totalFetches; //reset the evicted index LRU to current time
		}

		bool isPreHit = false;
		//implement prefetcher
		if(blockInd == (cacheSize - 1)){//check set if we're wrapping around
			for(int j = 0; j < ways; j++){
				if(setAssocCache[j][0] == nextTag){
					LRU[j][0] = totalFetches;
					isPreHit = true;
					break;
				}
			}

			if(!isPreHit){//there's no empty spot -> use LRU
				int victimInd = getVictimInd2D(trace.size(), ways, LRU, nextInd);
				setAssocCache[victimInd][0] = nextTag;//evict the victimInd by replacing with new tag
				LRU[victimInd][0] = totalFetches; //reset the evicted index LRU to current time
			}
		}else{
			for(int j = 0; j < ways; j++){
				if(setAssocCache[j][nextInd] == nextTag){
					LRU[j][nextInd] = totalFetches;
					isPreHit = true;
					break;
				}
			}

			if(!isPreHit){//there's no empty spot -> use LRU
				int victimInd = getVictimInd2D(trace.size(), ways, LRU, nextInd);
				setAssocCache[victimInd][nextInd] = nextTag;//evict the victimInd by replacing with new tag
				LRU[victimInd][nextInd] = totalFetches; //reset the evicted index LRU to current time
			}
		}
		totalFetches++;
		
	}

	vector<int> results;
	results.push_back(hits);
	results.push_back(trace.size());
	return results;
}

vector<int> setAssociativeCachePrefetchingOnMiss(vector<instruction> trace, int bitLength, int ways){
	unsigned int cacheSize = (int)(pow(2, bitLength - log2(ways)));
	//set up initial tag table with 0s and the LRU table with -1s
	vector<unsigned long long> tagTable(cacheSize, 0);
	vector<vector<unsigned long long>> setAssocCache(ways, tagTable);
	vector<int> LRUTable(cacheSize, -1);
	vector<vector<int>> LRU(ways, LRUTable);
	//now loop through trace and parse for tag and whether S or L
	int hits = 0;
	int totalFetches = 0;
	for(unsigned int i = 0; i < trace.size(); i++){
		unsigned long long blockInd = parseAddress(trace[i], bitLength - log2(ways))[0];
		unsigned long long tag = parseAddress(trace[i], bitLength)[1];
		unsigned long long nextInd = parseAddress3(trace[i], bitLength - log2(ways), 32)[0];
		unsigned long long nextTag = parseAddress3(trace[i], bitLength, 32)[1];

		bool isHit = false;
		bool doPrefetch = false;
		for(int j = 0; j < ways; j++){
			if(setAssocCache[j][blockInd] == tag){
				hits++;
				LRU[j][blockInd] = totalFetches; //update LRU
				isHit = true;
				break; //have to break to not double count
			}
		}

		if(!isHit){//implement LRU
			int victimInd = getVictimInd2D(trace.size(), ways, LRU, blockInd);
			setAssocCache[victimInd][blockInd] = tag;//evict the victimInd by replacing with new tag
			LRU[victimInd][blockInd] = totalFetches; //reset the evicted index LRU to current time
			doPrefetch = true;
		}

		if(doPrefetch){
			//totalFetches++;
			bool isPreHit = false;
			//implement prefetcher
			if(blockInd == (cacheSize - 1)){//check set if we're wrapping around
				for(int j = 0; j < ways; j++){
					if(setAssocCache[j][0] == nextTag){
						LRU[j][0] = totalFetches;
						isPreHit = true;
						break;
					}
				}

				if(!isPreHit){//there's no empty spot -> use LRU
					int victimInd = getVictimInd2D(trace.size(), ways, LRU, nextInd);
					setAssocCache[victimInd][0] = nextTag;//evict the victimInd by replacing with new tag
					LRU[victimInd][0] = totalFetches; //reset the evicted index LRU to current time
				}
			}else{
				for(int j = 0; j < ways; j++){
					if(setAssocCache[j][nextInd] == nextTag){
						LRU[j][nextInd] = totalFetches;
						isPreHit = true;
						break;
					}
				}

				if(!isPreHit){//there's no empty spot -> use LRU
					int victimInd = getVictimInd2D(trace.size(), ways, LRU, nextInd);
					setAssocCache[victimInd][nextInd] = nextTag;//evict the victimInd by replacing with new tag
					LRU[victimInd][nextInd] = totalFetches; //reset the evicted index LRU to current time
				}
			}
		}
		
		totalFetches++;
		
	}

	vector<int> results;
	results.push_back(hits);
	results.push_back(trace.size());
	return results;
}

int getVictimInd1D(long long traceSize, int ways, vector<int> LRU){
	long long victimVal = traceSize + 1; //set to be larger than any possible LRU index value
	int victimInd = 0;
	//check which way has LRU index
	for(int y = 0; y < ways; y++){
		if(LRU[y] == -1){
			return y;
		}

		if(LRU[y] < victimVal){
			victimVal = LRU[y];
			victimInd = y;
		} 
	}
	return victimInd;
}

int getVictimInd2D(long long traceSize, int ways, vector<vector<int>> LRU, unsigned long long blockInd){
	long long victimVal = traceSize + 1; //set to be larger than any possible LRU index value
	int victimInd = 0;
	//check which way has LRU index
	for(int y = 0; y < ways; y++){
		if(LRU[y][blockInd] == -1){
			return y;
		}

		if(LRU[y][blockInd] < victimVal){
			victimVal = LRU[y][blockInd];
			victimInd = y;
		} 
	}
	return victimInd;
}

void printResults(vector<vector<vector<int>>> resultVec, const char *argv){
	std::ofstream output(argv);
	for(unsigned int x = 0; x < resultVec.size(); x++){
		for(unsigned int i = 0; i < resultVec[x].size(); i++){
			for(unsigned int j = 0; j < resultVec[x][i].size(); j++){
				if(j != resultVec[x][i].size() - 1){
					output  << resultVec[x][i][j] << ",";
				}else{
					output << resultVec[x][i][j] << ";";
				}
			}
			if(i != resultVec[x].size() - 1){
				output << " ";	
			}
		}
		output << endl;
	}
	output.close();
}