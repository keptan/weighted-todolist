# weighted-todolist
grab a random todolist item, but weigh the items by importance. Combine with cron and  [mioSays](https://github.com/keptan/MioSays) for a desktop todolist reminder!

![Piped into Miosays!](https://raw.githubusercontent.com/keptan/weighted-todolist/master/exampleMio.png)

Replaces this script
```
IFS=$'\n'	#only use newlines are seperator
ACC=''

priorityGet () 
{
	for line in $(cat ./todo ./8ball) #loop over todolist
	do
		#append them multiplied by their priority
		num=$( echo "$line"  | cut -f 1 -d " ") 
		text=$( echo "$line" | cut -f2- -d " ")
		for i in $(seq 1 "$num"); do ACC+="$text"$'\n' ;done;
	done

	echo $(echo "$ACC" | sed "$ d" |  shuf -n 1) 
}

priorityGet
```
With >25x speed improvements. I imagine it could be done alot faster in bash, maybe appending to a temp file or something (replicate the range tree method even) . GNU shuf is really fast, but appending to a bash variable is stupid slow.

# compiling
fun lib relies heavily on c++17 features ```g++ priorityList.cpp -O3 -std=c++17  ```
