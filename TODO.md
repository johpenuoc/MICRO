## 16/11/26
### About 10
So far, as it is, the program tokenizes the file and then parses it. 

The function to parse is named as 'stack'. I named it as such because, in code, it functions less like a tree and more as a 2d array whereby the second dimension has 'stacks'. Like this:

[=:[x:[[2:[[/:[3:[7:::

Hence 'stack'.

The code works, at least in the limited testing its been given. So, assuming parse_expr and parse_assignment work correctly, the next steps should be to expand all of the terminal tokens/functions so that you can write more than simple assignments. 

This shouldn't be massively difficult. Largely, most code is comprised of expressions, statements, assignments, and the rest is a mix or only a little different (I'm only talking in the context of parsing). For example, [IF: [STMT: [CODE: [(optional)ELSE:. The STMT is a comparison (x == y?), which will use the same logic as parse_assignment, just opposite in order. 

That will be the next step. But as of right now, the basic tokenize system and basic parsing system works. Functionally it cannot actually execute the code right now, of course. However, that very difficult part will come soon.

### 11:30
parse_expr WORKS! I mean, I've tested it fairly thoroughly, though I'm sure it isn't completely robust and perfect. But maybe it is! Kinda scared to fully checked because I've gotten a 'programmers high' when code actually works. And strangely I understand it xD. Regardless, now that parse_expr works, I need to move onto the next stage of development: everything else! Expressions and assignments work, so I suppose the next thing would be to get statements working? Not entirely sure yet. Anyways, it's lunch time.

### 15:30
Pretty cool! Now, it understands what I call 'linkers'. That is player.x, conn.port, something.something_else.another_something_else. Pretty cool I think! The next step is to introduce functions. It should be able to recognise a function, its arguments, when it's in an expression and when its simply being called. It should also be able to handle expressions inside of the function. That is, arguments. function(2 + 4, player.x, conn.port, ...); I think it'll be more or less the same as the parse_expr and parse_IDlinker. Honestly, I believe most of it will be, considering that the algorithm is very generalised. 

Time to get coding! I'm plugged in.

### 16:34
Currently, the parse_func is proving to be slightly more difficult. I don't believe that's because of complexity, but rather my being not prudent; I hadn't thought it through very deeply. 

I will continue coding.

### 17:14
I believe it to be working right now. It's dinner time, and so I'm not going to be thoroughly testing it today. I'm sure there are logic bugs, but at the moment it works pretty fine.

## 17/11/26
### 8:41
Not going to develop much today. I have some classes and some work to finish. Perhaps later, but not too sure. I also have a test tomorrow, so we will see; I have to revise. 

If I do code today, I will be developing the current code: making it more robust. I know that it 'works' with simple lines of code, for example:

--> function(arg1, arg2, arg3.obj1, 1 + 4 / (8 * 0.5));

but thats it. Im not sure whether

--> function(arg1, function2(arg1, 1 + 4 * (0.5 - 1)), player.x);

would work. Hence making it more robust. We will see. I do believe that, if it doesn't work, a more thorough reworking of the program will have to commence. I did kind-of rush it, and I hadn't thought it through. I kinda just thought "I will simply copy-and-paste the code for parse_expr, change it slightly, and hope for the best" xD To be honest, most programming is 'hoping for the best'!

### 8:59
Nevermind. Nothing can stop the coding. Joking. I got everything done pretty quick. There's always far less than i anticipate.

### 9:07
Didn't expect to be back here again. However, pretty quickly, I managed to find the logic bug hiding in my code. Now, when a function is called, it can contain however many functions as arguemtns required, as the recursive system functions properly. 

Next is to see whether it works with linking...

### 9:20
I believe it is now working! So now it recognises:

player.box.movex(dt, ...); 

I do believe linking will also work with

func().something.something

however, I'm not sure I'm going to be implimenting that within my language. [FUNCTION: will not be a class or object which can have attributes. So [DECIMAL: won't do anything. It'll cause an error.

### 9:30
Next shall be... SCOPES.

The main idea being:

{} <-- this is a scope

It will be called in certain statements, but also all around the code -- not jsut in statements. 

[IF:[STMT:[SCOPE:[(optional)ELSE:

SCOPE will just be collecting a bunch of expressions and whatnot. Effectively, you can write anything in a scope. However, whatever was initialized in the scope will be deleted once out of the scope. Unless its writing to something initialized outside of the scope.

### 9:40
I'll carry it on later. REASONS.

### 10:35
I am back (was back about 15 minutes ago...). I've eaten and I've continued development of the 'parse_scope' function. So, it's essentially a rework of the 'stack' function. Essentially, a scope can kind-of be thought of as a mini-program. Seperate (not really but just how I'm thinking of it) from the rest of the program.

### 11:30
I GOT IT WORKING. I haven't really tested it and I'm rushing (I have a class in less than 30 minutes), but anyways, it appears to work. I'm not sure whether scopes in scopes will work, but I'll see to it later. For now, this is fine. Good work!

### 15:04
Just got back from a 2 hour class.

I've added STACK_TYPEs. They mark any NODES with a type like EXPRESSION or FUNCTION or ARGUMENTS. It'll make the interpreter easier to code and it won't have to figure out what anything is doing. It is explicitly stated.

### 15:18
The scan.c preprocessor is flawed. It allows for multiple spaces outside of a string. Got to fix that.

### 15:32
I got to get to class quickly. However, I did indeed fix the problem. 

Next will be IF statements.

### 18:46
Class was incredibly boring.

Anyways, not so much about programming per-say, however, I have come up with a better idea than 'Boloogna'... MICRO. Meaning Massively Important Coding Repository Origin. It's supposed to be a joke if you couldn't tell. It's neither massive, massively important, a massively important repository, or a massively important repository origin... FOR ANYTHING. It is, however, a 'coding' language. 

I should probably get to some actualy coding now, lul.

### 20:24
Apparently I've found another problem with scan.c. Basically, IT DOESN'T RECOGNISE EQUIVALENCE '=='. Genuinly no idea why. I hate running back over old code. I write it poorly, and thus I can never read it again. I won't be able to sleep knowing this is there.

### 21:29 
I have to get to bed. Anyways, I need to fix that RPAREN thing tomorrow, after my test...

## 18/11/26
### 10:56
I started coding maybe about an hour-ish ago. That was right after a test. I definitely aced it. Although, I was very tired and sleep deprived, so I messed up a stupid probability question that's easy as anything. Oh well. Nonetheless, I've fixed both of the previous problems. [IF: statements work properly too now, I think. I still haven't properly tested them, like if statements within if statements. Actually I'm going to do that neow.

### 10:58
It doesn't work.

### 11:01
It works.

Perphaps my next course of action is to impliment else statements? Shouldn't be too hard. All that is is checking for an option ELSE. If it exists, grab it, then save the scope afterwards. If it doesn't exist, exit. There is also the possibility that an else may contain an if afterwards, so if the next token is LBRACE, grab scope. If it is IF, grab another if statement. However, I'm not entirely sure on the best way to order these within the stack. I'll let the program decide and go about its own logic. Then I'll inspect it. If its easy enough to work with once the stack generation is completed, I'll simply leave it. It's not always worth engineering something that's good enough, in my opinion.

### 11:29
I have fully implimented if and else and else-if statements. You can embedd them, or do whatever.

It works similarly to C: if cond scope, else if cond scop, else scope.

### 11:33
Got a phone in the mail, sent by my lovely mom! Gonna go collect it!

### 14:25
The phone is great! It's a Pixel 10. I also had a chicken wrap, onion rings and chips. Yummers.

Anyways, I believe the next development has to do with the parse_condition. So far, it can read simple conditional statements like 2 * 3 == a, or b * a + (4 - 0.5) == c * 0.5, but currently there is no AND or OR or NOT in operation. So conditional statements are quite limited right neow. I think I'm going to implement that.

### 16:32
Haven't implimented it properly (it only works with single if statements). I've somehow broken it all and EVEN WHEN I REVERT BACK it still doesn't work.

### 16:47
I can finally be released from my prison; it works.

Apparently, SOMEWHOW, a little '-1' crept in. I'm not entirely sure how, but I knew that there were no logic bugs. Thus, this code was giving me a headache. Nonetheless, it now works.

AND and OR's can be used in conditional statements in if / if-else statements. 

It works!

### 18:37
I'm back from a food and walk break. I'm feeling pretty good about this.

Basically, what caused the error last time was the smallest of things.

When writing an mmstack to mstack, I was unfortunately not writing it to the furthest index, but instead I was writing it to the second-to-last one. I didn't notice this and I genuinely have no idea how it got in there. But, either way, the code is fully functional now; AND and OR statements work.

Next will be to impliment them with brackets.

### 20:14
The functions of parse_condition have been absorbed by parse_expr. So now, not only can you have "if (a & b | c) & e {}" or something, but you can also declare variables as "a = (b & c) - 1;". 

Pretty cool!

I just need to get the '==' operator working.

### 20:22
I got it fully working! Well, I'll test it more thoroughly after I've gotten a drink. However, so far, it appears to be functional.

### 20:31
I've tested it and it seems to work!

### 20:41
Next-up for development: nots, gthan, lthan

### 20:58
I got the latter two working. NOT... not so far xD. Anyawys, off to bed.

## 19/11/26
### 15:03
I will continue developement on the NOT token. 

I want it to work very simply:
- ! before anything inversts it (!a)
- ! before '(' inverts the expression.
- An invert is to change true to false 
and false to true.

### 15:16
I think I've implemented it? I mean, I used the same logic as LPAREN and RPAREN. You just have to surround whatever it is you're inversing with '()'. If it comes accross a NOT, it just checks for LPAREN next and calls another parse_expr function. Seems pretty simple. Hopefully it's robust!

Now I wonder what is next...

Perhaps a while loop? Or for loop? Or do I just still with a more general 'until' loop?

I think I'm going to go with defining functions!

Time to continue development.

### 15:28
I've also decided on implementing the 'return' token.

### 15:55
It appears that I have implemented the ability to define functions neow. It was quite simple, but many things like this will be. For example, a while loop is checking for a while token, creating a NodeMaster with 'while' as its type, then taking a condition, then a scope, and thats it. The hard part is actually running through the code... But this part is quite easy. Unless you've got to search for a '-1' in 1000 lines of code, that is.

Now I suppose I'll implement the 'until' loop? Why not. Boring but easy and might as well get it done.

### 16:09
THE PARSE_CONDITION FUNCTION ISN'T WORKING AGAIN.

### 16:10
Nevermind :)

### 16:23
Okay, so UNTIL loops have been implemented and defining functions has been too. Didn't take to long, but many things from here-on-out (at the stack level) are merely arranging current functions in different orders.

Gonna go on a walk and grab dinner. The next thing for development will probably be the RETURN token.

### 20:49
I've been out and gone for a little while. Even when I've been in the library, I've just sat and done nothing. Had a wave of motivation, of course. Obviously I had the total opposite. I'm barely recovering right now. Anyways, this death of motivation has strangely given me some clarity; my code is very messy, unorganised, and I've realised that I've been using an array called 'mem', yet kind of used it and kind of haven't? Very strange. Anyways, I've implemented the tokens 'ADD_EQ' and for sub, star and div, of course. So you can now do 

i += 1;

rather than

i = i + 1;

Next shall be... lists!

### 21:11
Lists have been defeated and implemented!

Time to log off :)

### 22:57
I know I said I was logging off but I still have lots of energy.

Anyways, I implemented list accessing, like list[index, index2:.

## 20/11/26
### 14:59
I've just logged on. I'm trying to not make this a diary, but it's somewhat become one...

Next up for development are classes!

"cls Something :: OtherSomething
{
    @ _init
    {
        this.obj1 = "hello";
        this.obj2 = 10;
    }
    
    fn change_obj1(change_to)
        {
            this.obj1 = change_to;
        }
}

a = Something;"

This is how they're going to be structured.

### 16:14
Classes have now been implemeneted!

### 16:22
I've implemented a lot into the parer; classes, functions, function calls, if-else statements, loops, linkers, expressions, variables, scopes... So perhaps I should start on the next stage: processing this stack-data into commands for the interpreter to go through!

### 16:35
=== planing how to compile this properly ===



============================================

### 19:17
I went and ate, walked around, but since then I haven't been coding. Just thinking.

I do have an idea.

The stack is going to be converted into a stream of 'commands'. For example,

x = y * 3;

This would be converted into the stack

:=::x:::*::y::3::

Then this would be converted into the stream

LOAD y, MUL mem:y: 3, PUSH, STORE x

'LOAD' loads something from memory, 'MUL' multiplies two numbers, 'PUSH' puts the last value into memory, 'STORE' puts the latest item in memory and stores it in whatever variable.

 x += function(y) * (5 - 0.5);
 
 LOAD (x), MUL ((LOAD (y), PUSH, LOAD (function)), PUSH, SUB (5 -0.5), ),... I can't be bothered to do the rest, but you get the idea.
 
 or maybe...
 
 x = y * 3;
 
LOAD y
PUSH 3   
MUL      
STOR x

a = y * (func(x) * 0.5); -->
STO , a , MUL , LOAD , y: MUL , FNCALL , x: LITERAL , 0.5:::

### 20:58
Okidokie. I've thought this through a little.

There will be a CMD struct, which contains a type, how many arguments it expects, and the token associated with it.

When initially reading the raw 'bytecode' (it won't be quite bytecode at this stage), it will use ':' to determine when the arguments for a COMMAND has ended. Similar to LPAREN and RPAREN in expressions. Once I get it to this stage, I will convert it into true bytecode. This is an intermedite stage between bytecode and the stack. 

### 23:35
I have NO idea why the code isn't working. Been looking at it for ages. It quite literally makes ZERO sense as to why it isn't running... Why can't I access any index in cmds???

## 21/11/26
### 15:16
WE'RE MAKING BYTECODE BABYYYY

### 16:57
Greetings. As it is, we now can create bytecode for very simple expressions. Take:

a = y * (func(x) * 0.5);

this gets converted into the stack

STACK: :ASSIGNMENT:
                NODE: :EQUAL:-------->:=:
                NODE: :IDENTIFIER:--->:a:
                STACK: :EXPRESSION:
                        STACK: :OPERATION:
                                NODE: :STAR:--------->:*:
                                NODE: :IDENTIFIER:--->:y:
                                STACK: :EXPRESSION:
                                        STACK: :OPERATION:
                                                NODE: :STAR:--------->:*:
                                                STACK: :FUNCTION_CALL:
                                                        NODE: :IDENTIFIER:--->:func:
                                                        STACK: :ARGUMENTS:
                                                                STACK: :EXPRESSION:
                                                                        NODE: :IDENTIFIER:--->:x:
                                                NODE: :NUMBER:------->:0.5:
                                                
which gives us the (unfinished and not ready) pseudo-bytecode:

_PUSH, STACK:2:, INT,

_ID_PUSH, STACK:6:,

_CALL, HASHMAP:FUNC:,

_MUL, STACK:8:, STACK:7:,

_ID_PUSH, STACK:6:,

_MUL, STACK:8:, STACK:7:,

_ID_PUSH, STACK:6:,

_STO, STACK:4:, STACK[7:,

Sweet!

### 20:05
I've been getting on for the past hour or so the development of converting 'stack', or AST tree' into bytecode. It's in a very simple and primitive state, but I was just wanting to get something working that's fairly coherent. I've been laying out how the byte code will work. So far, it works pretty alright!

### 20:26
Next will be to add more commands and to make them more cohesive and generally better!

Going to take a break neow.

BTW, every ':' should be a '[' or ']', but it was messing up the .md formatting.

### 21:25
Okay, I've got it figured out!

We don't reverse the entire tree, but we only rever each sub tree!

### 22:40
It now generates bytecode for pretty simple programs.

Next will be to simply expand that! I will need to add more commands like 'JUMP' so 'until' loops and if-else-statements can work. Then afterwards I will have to build the VM to actually run that code. I can make multiple VM's for multiple platforms, making my language portable too!

I'm not sure whether to begin development on that now. It is a little late, though I'm not tired. I'm just very aware that the following developments will probably take some time. 

## 22/11/26
### 15:39
I have come to a halt.

As it is, if I loop through the stack backwards, for some operations it works perfectly. For others, it messes it up. This is proving to be a little more difficult than I thought.

More planning is needed.

### 15:43
Okay, so I have an idea.

We loop through each 'NODES', but if the 'NODES' contains 'NODE's, then we don't dig in any further.

Inside (where we don't dig any further), we use more recursion to loop through. This will allow function calls and loadings of arguments to happen in the correct order. For example:

if outside_type == FUNCTION_CALL
    recursive:ARGUMENTS NODES: <-- the arguments will be loaded in this recursion
    aastream:CALL:;
    
Actually... I'm not sure if that's compatible with everything. For example, it would make working with scopes incredibly difficult now that I think about it...

I need to plan this more thoroughly.

### 16:16
Bytecode planning...

### 16:26
So I am going to make a function specifically for each possible scenario. For example:

Say we are looping through the stack and we hit an ASSIGNMENT NODES, then we will call bytecode_assignment:stack->UNION.Nodes.node[i]:

At that point, it will give me pinpoint control over what happens during each phase of recursion.

Each function will simply consist of a switch statement and more recursion.

I will implement this later. But I think I have it planned.

Also to state some rules:

HASH -> takes the last value in the stack and uses that as a key
     -> it then consumes that value and then retrieves the next value
     -> it then stores that value in the hash table at the respective key
     
LOAD -> takes the last value in the stack and uses that as a key
     -> it then uses it in the hash table and loads the value onto the stack
     
PUSH -> pushes the last value onto the stack

### 16:43
I've made a primitive and fairly crude start to my implementation. So far, in my head at least, it seems as though it'll work. 

I'm going to head to Church neow. After that and a good meal, I'm going to lock in. By the end of today, some good BYTECODE is going to be generated!

## 23/11/26
### 8:13
Last night I had found some problems that I didn't see before. For example, the parser wouldnt understand 

player.box.centre = new_x; 

or something. Anyways, I fixed that!

Time to continue with the bytecode generator.

### 8:25
If we have something like 

x = 5;

'5' will be sorted in bytecode as:
    LITERAL INT 5,
    PUSH,
    STO x,
    
### 10:12
Okidokie. I have completed the initial, primitive design. It works for very simple assignments like

a = 5; name = "john'...

But, now that the foundations have been laid, the rest should flow quite nicely!

Continuing work.

### 10:24
I have found another bug with parser.c. 

list[6] = ...;

This doesn't work. Going to pause compiler development and patch issue.

### 10:38
I've fixed the issue. However, it is still present in parse_scope. I'll deal with that later.

Back to compiler.c development.

### 10:45
Regarding the bytecode generation for ID linking:

player.box.centrex

in bytecode, this may look like

-- we push each key onto the stack in reverse order
-- we then use LOAD_SEQ 3
-- 3 indicates how many items from the stack we use 
-- LOAD_SEQ places each key into the next loaded hashmap
PUSH centrex
PUSH box
PUSH player
LOAD_SEQ 3
PUSH

### 12:04
Okidokie. The previous note has been implemented.

Given player.data.box.x = 0.5; in the .mcr file, we generate the bytecode:

0.500000 FLOAT,
PUSH

x STRING,
PUSH

box STRING,
PUSH

data STRING,
PUSH

player STRING,
PUSH

LOAD_SEQ 4

Pretty cool!

next will be to convert expressions into bytecode!

I'm going to go grab lunch neow.

Oh, something to note is that the bytecode_IDlinker() will also have to check for certain node types, mainly FUNCTION_CALL. You may have player.box.move(); or something, but I will implement this later.

### 13:25
I'm back from eating my lunch. Now is the time to continue development. First order of business: calling a function at the end of an ID link.

Beginning development neow.

### 13:45
Development has been going well, but I've found another bug in parser.c. 

Patching neow.

### 13:57
Fixed it. But it's still not pathed for parse_scope, but I'll do that later.

Continuing development with that compiler.

### 14:10
The linker is working -- still not for function calls though. I have, however, set it up so that will be easy to implement.

Continuing development.

### 15:06
It has compiled. Theoretically, it 'should' convert code such as

player.box.move() 

into byte code that'll look like

box STRING,
PUSH

player STRING,
PUSH

LOAD_SEQ 2

CALL move

--------------------
LOAD_SEQ will be loading a hashmap stored at box, whereby the key 'move' exists somewhere in there. At that point, the VM will implicitly understand that the previous searches through the hashmap relate to the current function CALL, and will look there first before looking at the general hashmap.

I'm a little tired, so ima go on a walk, buy an energy drink, and let my mind rest. I could've done with more sleep...

### 15:54
I'm back. I ate a snickers, drank some cherry redbell, and walked a fair little bit. Time to lock in.

### 17:17
Okidokie, I believe that it logically works, but I've messed up the bstream vs stream stuff. I've got to redo the fundamental ByteStream struct to fix this...

### 17:46
It works! I had to re-create the ByteSrream struct, but now it's working! TIME TO EAT!! 

## 24/11/26
### 8:53
I a got up a little later due to a workout last night at the GYM. Anyways, time to begin developing again.

Currently, the way it loads something like player.box.move() is by loading each successive object onto the stack, and then loading in a hashtable with each respective object. It then calls the final function. Here's how 'player.box.move();' looks:

box STRING,
PUSH

player STRING,
PUSH

LOAD_SEQ 2
CALL move

However, I'm unsure if this is correct. Because inside a hashtable, a function -- in my design -- will be nothing more than a bytecode address, which the VM will read and 'goto'. hen it will begin executing line-by-line until it finishes. It'll then return to the position it was originally in. 

SO perhaps I want to include 'move' in the LOAD_SEQ? If it is a function, too, I can load in the arguments beforehand. LOAD_SEQ consums each of the PUHSes of each key, so then after calling the function, the top of the stack will contain the arguments.

Gonna begin developing this.

### 9:13
I believe this has been implemented. Now, the function object at the end of something like player.box.move(), move is included in the LOAD_SEQ. Ontop of that, I have ordered the logic such that arguments will be loaded first, but I haven't actually reached that logic yet. I will begin implementing that now.

### 9:34
player.box.move(somearg, linkerarg.links, functionarg(), functionwithargs(arg1));

temporary storage.

### 9:45
It now can handle simple identifier arguments. However, it does it in an order I didn't plan for; it now loads in the arguments AFTER LOAD_SEQ. Technically it should be fine, but I'm going to change it anyways. Here's what it looks like:

move STRING,
PUSH

box STRING,
PUSH

player STRING,
PUSH

LOAD_SEQ 3

LOAD somearg
PUSH

CALL move

### 10:34
Okidokie. I got it working. It loads the arguments before loading in the objects for hashing. AND I re-did the ByteStream struct again. It was getting a little confusing. Nonetheless, It's now been simplified and slightly optimised. Though, true optimisation will be happening when I completely rewrite the entire codebase. This is just to get something working. Performance is not something I care about right now, especially considering I don't know much about C and it's my second project.

### 12:00
I'm back! Time to continue developing processing arguments!

### 12:27
It can now load identifiers, literals, and so the next step will be to load functions and stuff.

### 12:40
Okidokie. As it stands right now, the compiler can take arguments in a function which range from literals to identifiers to linkers. For example:

player.box.move(somearg, "name", 0.5, 4, player.x);

This gets converted into:

LOAD somearg
PUSH

"name" STRING,
PUSH

0.500000 FLOAT,
PUSH

4 INT,
PUSH

x STRING,
PUSH

player STRING,
PUSH

LOAD_SEQ 2

PUSH

move STRING,
PUSH

box STRING,
PUSH

player STRING,
PUSH

LOAD_SEQ 3

PUSH

CALL

Next will be to get the recursion really working; we will pass functions as arguments into other functions, which themselves may contain arguments which could be even more functions!

Continuing development now.

### 13:14
The recursion is working. A function can take many arguments now. The only thing it can't take yet are lists and list-indexes, so either :1, 2, "name"...: and list:index:, but ill get to that now.

### 13:35
I have implemented a 'CONSUME' thingy. Basically, when said after a STO function, it basically takes the previous item in the same (hence consume) and uses that as what to assign to a variable.

### 14:08
I have updated the bytecode_assignments() too now. They work great! So functions, arguments in functions, linkers, assignments, literals, variables -- they are all able to be converted into bytecode nicely now. 

The bytecode is a little lengthy, but I'm purposely using as little commands as I can.

Next up for development: lists and list accessing.

Continuing development.

But first, what would a list look like in bytecode...

LIST ACCESSING:
...............
I'm thinking we do the same thing as we did with linkers:

list[0, 2]; -->

2 STRING
PUSH

0 STRING (we convert it into a string)
PUSH

LOAD list
PUSH

LOAD_SEQ 3
PUSH

Why can't a list simply be a multi-dimensional hashmap underneath with multiple values? Idk, I think I'm just going to stick with doing it this way.

LIST CREATING:
..............
As a list is basically just a multi-dimensional hashmap under-the-hood, I just need to create a spot on the hash map and essentially append each 'argument' into it. That may look like:

list = [1, 5, "name"]; -->

1 INT
PUSH

5 INT
PUSH

name STRING
PUSH

STO_SEQ list 3

Lets get developing!

### 14:48
You can now define lists!

list = [...]

Pretty cool mannnn.

Time to develop list-index-accessing.

### 18:53
I am back and continuing work.

At the moment, the lists work, however they do not work when assining an index of a list to another list. 

Going to fix this bug.

### 19:08
Okidokie. I believe I've gotten it working!

### 19:56
It appears that I've gotten it to a very good start! Scopes now work, you can call functions, have arguments, use lists, indexes to lists, linkers, assignments, and all of that! Next will be if statements, for loops, defining functions, defining classes, calling classes, and some other things!

Taking the source code:
myfunction("something", idsomething, list[7, 2]);

{
    a = player.box.move(world.centre, 0.4, "true");
}

It generates:
SCOPE_START


"something" STRING, PUSH

LOAD idsomething, PUSH

7 INT, PUSH

2 INT, PUSH

list STRING, PUSH

LOAD_SEQ 3, PUSH

LOAD myfunction, PUSH

CALL

SCOPE_START


centre STRING, PUSH

world STRING, PUSH

LOAD_SEQ 2, PUSH

CALL

0.400000 FLOAT, PUSH

"true" STRING, PUSH

move STRING, PUSH

box STRING, PUSH

player STRING, PUSH

LOAD_SEQ 3, PUSH

CALL

STO a, SCOPE_END


SCOPE_END

Pretty cool!!
Logging off for tonight.

### 21:15
I'll be actually logging off soon, but I just wanted to read through some code and hypothesize some things. I'm not really adding to the codebase that much, if anything.

Just to outline the full processing:

Tokenization:
[IDENTIFIER]     myfunction
[LPAREN]         (
[STRING]         "something"
[COMMA]          ,
[IDENTIFIER]     idsomething
[COMMA]          ,
[IDENTIFIER]     list
[LBRACKET]       [
[NUMBER]         7
[COMMA]          ,
[NUMBER]         2
[RBRACKET]       ]
[RPAREN]         )
[SCOLON]         ;
[LBRACE]         {
[IDENTIFIER]     a
[EQUAL]          =
[IDENTIFIER]     player
[DECIMAL]        .
[IDENTIFIER]     box
[DECIMAL]        .
[IDENTIFIER]     move
[LPAREN]         (
[IDENTIFIER]     world
[DECIMAL]        .
[IDENTIFIER]     centre
[COMMA]          ,
[NUMBER]         0.4
[COMMA]          ,
[STRING]         "true"
[RPAREN]         )
[SCOLON]         ;
[RBRACE]         }

STACK: [PROGRAM]
        STACK: [FUNCTION_CALL]
                NODE: [IDENTIFIER]--->[myfunction]
                STACK: [ARGUMENTS]
                        STACK: [EXPRESSION]
                                NODE: [STRING]------->["something"]
                        STACK: [EXPRESSION]
                                NODE: [IDENTIFIER]--->[idsomething]
                        STACK: [EXPRESSION]
                                STACK: [LIST_ACC]
                                        NODE: [IDENTIFIER]--->[list]
                                        STACK: [LIST]
                                                STACK: [EXPRESSION]
                                                        NODE: [NUMBER]------->[7]
                                                STACK: [EXPRESSION]
                                                        NODE: [NUMBER]------->[2]
        STACK: [SCOPE]
                STACK: [SCOPE]
                        STACK: [ASSIGNMENT]
                                NODE: [EQUAL]-------->[=]
                                NODE: [IDENTIFIER]--->[a]
                                STACK: [EXPRESSION]
                                        STACK: [ID_LINKER]
                                                STACK: [ID_LINKER]
                                                        NODE: [DECIMAL]------>[.]
                                                        STACK: [ID_LINKER]
                                                                NODE: [DECIMAL]------>[.]
                                                                NODE: [IDENTIFIER]--->[player]
                                                                NODE: [IDENTIFIER]--->[box]      
                                                        STACK: [FUNCTION_CALL]
                                                                NODE: [IDENTIFIER]--->[move]
                                                                STACK: [ARGUMENTS]
                                                                        STACK: [EXPRESSION]      
                                                                                STACK: [ID_LINKER]
                                                                                        STACK: [ID_LINKER]
                                                                                                NODE: [DECIMAL]------>[.]
                                                                                                NODE: [IDENTIFIER]--->[world]
                                                                                                NODE: [IDENTIFIER]--->[centre]
                                                                        STACK: [EXPRESSION]      
                                                                                NODE: [NUMBER]------->[0.4]
                                                                        STACK: [EXPRESSION]      
                                                                                NODE: [STRING]------->["true"]
                                                                                
SCOPE_START
"something" STRING, PUSH

LOAD idsomething, PUSH

7 INT, PUSH

2 INT, PUSH

list STRING, PUSH

LOAD_SEQ 3, PUSH

LOAD myfunction, PUSH

CALL

SCOPE_START
centre STRING, PUSH

world STRING, PUSH

LOAD_SEQ 2, PUSH

CALL

0.400000 FLOAT, PUSH

"true" STRING, PUSH

move STRING, PUSH

box STRING, PUSH

player STRING, PUSH

LOAD_SEQ 3, PUSH

CALL

STO a, SCOPE_END
SCOPE_END

On github, it won't format so well. But the data and information is, indeed, present.

How should an if-statement be represented in bytecode..? I think I will have to introduce a new command: JUMP, but specifically whether a certain condition is true or not. This will also allow us to tackle loops, which will be the first order of things.

Did a little more than I meant... going to bed now, for real.

## 25/11/26
### 9:45
I've been developing for near-enough the past 45 minutes. Loops are nearly complete for conversion into bytecode, however I need to implement the JUMP command next, which will complete the loop-to-bytecode conversion!

Continuing development.

### 9:49
I have to track each bytecode command now for the jump command to know where to actually jump to.

### 10:15
I have implemented the jump command!

I'm very hungry and have class soon, so I'm going to go, but that is some pretty decent work!

### 11:24
I will be implementing better tackling of multiple scenarios and stacks within conditions and whatnot. Essentially making it more robust.

Continuing development.

### 12:40
I stumbled upon some bugs in the parser. I've patched them. It had problems with list accessing in conditions and scopes. I believe its completely fixed but unsure currently. When I go over everything to optimise, I will improve the current state of it and make it more readable and reliable. And, of course, any bugs left will be patched.

### 12:59
Okidokie. I've gotten loops to somewhat work in their conversion to bytecode, and I've patched some general bugs in the compiler, not to mention to pretty huge bugs in the parser. Nonetheless, this was mainly a patching session as it seems...

### 14:06
This took me ages, but I was patching SO many bugs in the parser... things like:

var1 = link1.link2[5, 4] + link3[6];

Anyways, it appears to work now!! IM RELEASED FROM MY PRISON! How well it's converted into bytecode... no idea! Because I've barely been developing that side of things! But I'll be checking it now.

Continuing development. Or maybe I'll go grab lunch.

### 15:23
I'm back, but my laptop is on 14% and the only plug available is by some girl... I may have to move rapidly.

This mustn't impede development and progress.

### 15:37
A plug has been secured and the bytecode translation is going well! Here:

a = 1;
until !(link1.obj < 10)
    {
        something = variable[2] + b[1];        
        idk = [func(2)];
    };

This gets translated into the bytecode:

SCOPE_START

1 INT, PUSH

STO a, obj STRING, PUSH

link1 STRING, PUSH

LOAD_SEQ 2, PUSH

11 INT, PUSH

LESS_THAN
PUSH

INVERSE
PUSH

JUMP_IF_FALSE 26,
SCOPE_START

2 INT, PUSH

variable STRING, PUSH

LOAD_SEQ 2, PUSH

1 INT, PUSH

b STRING, PUSH

LOAD_SEQ 2, PUSH

ADD
PUSH

STO something, 2 INT, PUSH

LOAD func, PUSH

CALL

STO_SEQ idk,
PROVIDE 1,
SCOPE_END

JUMP_IF_TRUE 23,
SCOPE_END

Pretty cool!

Time to being development on if-statements.

### 16:38
I thought it'd be wiser to complete some basic translation functions, like being able to translate '+=' and '/=' first. I have succeeded by using 'FLAGS' tagged onto the end of every CMD. But for some reason, I'm getting random STO_MUL commands??? No idea where they're coming from.

Patching now.

### 16:41
Nevermind, it was a printing error xD

Logging off and going to grab some food.

### 17:57
I'm back and it's time to begin developing some bytecode for if-statements!

### 20:24
Idk why I thought that'd be exciting. Anyways, it's not 100% done but its somewhat working. Here:

if (x > 5)
    {
        a += 1;
    }
else if (x < 5)
    {
        a += 2;
    }
else if (x > 15)
    {
        a += 2;
    }
becomes

SCOPE_START
LOAD x,
PUSH
5 INT,
PUSH
GREATER_THAN
PUSH
JUMP_IF_FALSE 7,
SCOPE_START
1 INT,
PUSH
STO a,  __ADDEQ__,
SCOPE_END
JUMP_IF_TRUE 27,
LOAD x,
PUSH
5 INT,
PUSH
LESS_THAN
PUSH
JUMP_IF_FALSE 7,
SCOPE_START
2 INT,
PUSH
STO a,  __ADDEQ__,
SCOPE_END
JUMP_IF_TRUE 14,
LOAD x,
PUSH
15 INT,
PUSH
GREATER_THAN
PUSH
JUMP_IF_FALSE 7,
SCOPE_START
2 INT,
PUSH
STO a,  __ADDEQ__,
SCOPE_END
JUMP_IF_TRUE 1,
SCOPE_END

I'm logging off...

### 20:49
I'm back. I'm going to implement that else statement now in bytecode.

Continuing development.

### 20:55
I've implemented them. Next will be defining functions, I suppose?

I'm thinking that we use different ByteStreams for functions and classes. We can then index them and, when called, we simply go to that index. It also means we don't have to worry about jumping to different lines; it simply pauses at the current bytecode address, goes to the respective bytestream function index, and executes that. Once completed, it resumes! Same for classes, too.

Beginning development.

## 26/11/26
### 9:43
I'm now going to fully implement defining functions and defining classes in bytecode translation/conversion.

Continuing development.

### 9:52
I'm thinking that a function's bytecode not only be in a different stream, but that also its information, like argument functions, be 

### 12:12
I suppose I forgot I was writing in the TODOD xD. Anyways, The code was going well, until I realised that I may need to implement a hashmap. That's fine as it isn't incredibly complex or anything, but it was rather how it took me off guard. I was expecting to do that later, so I've been thinking of how to harmonize all of the functions together now. 

I'm going to eat lunch. I'll have a think during that time. But I think I need to develop a hashmap first.

### 13:24
I've been thinking and I've realised that a hashmap is just overengineering. Rather than a hashmap, I can create a function name list. This will contain a function name and the bytecode for its scope. When a function is called, we run CALL, as usual, but we add an address to the end of it and a flag stating whether it's a function or class. We then use that number and flag to go to the correct bytestream list and retrieve the bytestream to execute!

Going to implement this now.

### 14:37
Okidokie, I think I've completed function defintions and calls now. 

Basically, when a function is defined, it saved the scoep and name with an index in a seperate bytestream. Then, when a function is called, it takes the names, searches through that list, pulls the index, and returns the bytecode 'CALL :index: FNCALL'. It will then load whatever index that is from the list, pull the scope, execute, and then return to the main bytestream.

Next will be classes. They will be done in generally the same way. Except the main difference being that the classes, when called, execute the initialization (@ init {}). The initialization contains not only the objects, but there must also be a bytestream stored in there specifically for clas function calls. This will also mean I need to reconsider linkers, maybe? Or maybe not. I do forget that a lot of things are handled behind the scences here that will also be using context to figure things out.

That will be next up for development. However, I'm going to take a small break.

### 15:01
I'm back! Time to begin developing again.