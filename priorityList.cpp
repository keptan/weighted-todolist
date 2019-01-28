#include "fun/list.h" 
#include "fun/string.h" 
#include "fun/sort.h"
#include "fun/tree.h"
#include "fun/range.h" 
#include <iostream>
#include <tuple>
#include <string>

//pair a range to a string, while allowing us to compare with an int
//for searching in a tree later
struct RangeString
{
	const Integers range; 
	const String string; 

	bool operator > (const RangeString& b) const
	{
		return range > b.range;
	}

	bool operator < (const RangeString& b) const
	{
		return range < b.range;
	}

	template<typename T>
	bool operator > (const T& b) const
	{
		return range > b;
	}

	template<typename T>
	bool operator < (const T& b) const
	{
		return range < b;
	}

	RangeString( const Integers i, const String s)
		: range(i), string(s)
	{}
};


//takes a list of strictly formatted todo list files 
//example file:
/*
 * 1 do dishes
 * 99 save the world
 */ 
int main (int argc, char** argv) 
{
	const auto toString = [](const auto c){ return String(*c);};

	const auto fileNames	= IteratorStream(argv + 1, argv + argc) | map(toString); //argv to strings 
	const auto fileStreams	= fileNames | map( FileLineStream); //filenames to file stream 
	const auto lines		= fileStreams |  Flatten(); //flatten files into a single stream of lines 

	//lines to a tuple of priority and todo item 
	const auto priorityPair = lines | Filter([](const auto l){return l.length() > 3;}) 
							| Map( [](const auto l) -> std::tuple<int, String>
								{
									//throws if isn't following the todolist format
									const int rank		= std::stoi(String(l.split(' ',1)[0]).string().c_str());
									const String str	= l.split(' ',1)[1];
									
									return std::make_tuple( rank, str);
								});

	//everything before this lazily evalulated here
	//filter out suspiciously short lines
	//build a tree of Integer Ranges paired to a todolist string
	//ex: 0 - 1 to dishes, 2 - 100 save the world
	//later when we use uniform dist to pick a random range, it will be much more likely we get assigned 'save the world' than 'do dishes'
	const auto RangeSet		= (priorityPair | Fold( [](const auto tuple, const auto acc) -> std::tuple< Tree<RangeString>, int>
								{
									const int floor		= std::get<1>(acc);	
									const int ceiling	= std::get<1>(acc) + std::get<0>(tuple);
									const String string = std::get<1>(tuple);
									const auto accTree	= std::get<0>(acc);


									return std::make_tuple(	accTree.push( RangeString( Integers(floor + 1, ceiling), string)), 
											ceiling);
								}, std::make_tuple( Tree<RangeString>(), 0) )).eval() ;

	//get a random int between 1 and the sum of priorities
	const int random		= (UniformDist(1, std::get<1>(RangeSet)) | Take(1) ) .get();

	//select, and output a todolist item, weighted by priority
	std::cout << std::get<0>(RangeSet).get(random).value().string;
}
