#/bin/bash
./tscancode --xml --quiet ../mcp/ 2>resault.xml

key="<error file"
error=false
while read line
do
    echo $line
	value=${line: 0: 11}
	
	if [ "$value" == "$key" ]
	then
		error=true
	fi
done < resault.xml	#TsCanCode resault

if [ "$error" = true ]	#not comply with the rules 
then
	exit 1
else
	exit 0
fi
