The program can be used to test the recursion execution of uintr handlers.
The program sends multiple user interrupts to itself.
The uintr handler in the program takes a long time to run.
If one uintr handler is running, the following user interrupts are blocked.
Therefore, the recursive execution is disabled in default.

To resolve this issue, we can call `_stui()` manually in the uintr handler to unblock the following user interrupts. 
In this way, the uintr handlers can be executed recursively.  

To test the effectiveness of manual unblock, there are two modes to run the program: one without `_stui()`, the other with `_stui()`. 

#### - Run with `./main` (without `_stui()` called in uintr handler)

The output will be like 

```
ndler 0 start
handler 0 end
handler 9 start
handler 9 end
handler 8 start
handler 8 end
handler 7 start
handler 7 end
handler 6 start
handler 6 end
handler 5 start
handler 5 end
handler 4 start
handler 4 end
handler 3 start
handler 3 end
handler 2 start
handler 2 end
handler 1 start
handler 1 end
10 UIPIs sent, 10 received
```

This means that uintr handlers don't execute recursively.

#### - Run with `./main stui` (with `_stui()` called in uintr handler)

The output will be like

```
handler 0 start
handler 3 start
handler 2 start
handler 1 start
handler 4 start
handler 5 start
handler 6 start
handler 7 start
handler 8 start
handler 9 start
handler 9 end
handler 8 end
handler 7 end
handler 6 end
handler 5 end
handler 4 end
handler 1 end
handler 2 end
handler 3 end
handler 0 end
10 UIPIs sent, 10 received
```

This means that uintr handlers execute recursively. 
