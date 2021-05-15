# Mail System

This is a mail system supporting managing and searching.

## Mail format
Each mail has the following information

```
<from>       := <string>
<to>         := <string>
<date>       := {YYYYMMDDhhmm} or {blank}
<expression> := <keyword> or (<expression>) or !<expression>
or <expression>|<expression> or <expression>&<expression>
<keyword>    := <string>
<string>     := {any alpha-numeric string without whitespace or punctuation}
```

## Supporting Operation

### Add
Add mail from a address.
```
add ./address
```

### Remove
Remove a mail by its ID
```
remove <ID>
```

### longest
Return ID of the mail with longest string

```
longest
```
### query
Search and return ID of the mails fit the conditions.

The following can be searched
```
1. Sender     [-f"<from>"] 
2. Receiver   [-t"<to>"] 
3. Date Range [-d<date>~<date>] (can be empty in either sides) 
4. <expression> include at most 20 keywords and can use union and intersection operations
```

The following are some example commands:
```
query -t"dsa14spring" -d201404252230~201405141300 final|project
```
```
query (nested&(parenthesis|(is&!good)))
```


## How to Use

First, build the project
```
make mail
```

Second, run the program
```
./mail
```
